#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include <input/sony_controller_input_manager.h>

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	font_(NULL),
	world_(NULL),
	audio_manager_(NULL),
	sfx_id_shoot(-1),
	sfx_id_move(-1),
	sfx_id_explode(-1)
{
}

void SceneApp::Init()
{
	//sprite renderer for drawing sprites on the screen
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	//input manager handles the controls
	input_manager_ = platform_.CreateInputManager();

	//initialise audio manager to add sound effects and music
	audio_manager_ = gef::AudioManager::Create();

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	//bool for checking if player has pressed L2 while moving
	L2Pressed= false;

	//Initialised so turret points up at start of game
	rightStickX = 0;
	rightStickY = -1;

	//vita screen dimensions
	screenHeight = 544;
	screenWidth = 960;

	//Load sound effects and music
	if (audio_manager_)
	{
		sfx_id_shoot = audio_manager_->LoadSample("pew.wav", platform_);
		sfx_id_move = audio_manager_->LoadSample("print.wav", platform_);
		sfx_id_explode = audio_manager_->LoadSample("explode.wav", platform_);
		audio_manager_->LoadMusic("music.wav", platform_);
	}

	// initialise the physics world
	gravityAmount = 9.81f;
	b2Vec2 gravity(0.0f, -gravityAmount);
	world_ = new b2World(gravity);

	///Initialise game manager and load the first level
	gameManager = new GameManager(world_, primitive_builder_, audio_manager_, sfx_id_shoot, sfx_id_move);
	gameManager->LoadLevel();

	//initialise player
	player = new Player(world_, primitive_builder_, gameManager->GetStartPosition(), audio_manager_, sfx_id_shoot, sfx_id_move, sfx_id_explode);
	playerHealth = player->GetHealth();
	playerSpeed = 6;

	InitBackground();

	//Loads all requires pngs and set textures materials and sprites
	InitTextures();
	
	//Creates the front  menu
	menu = new Menu(screenWidth, screenHeight);
	
	menuSprite.set_texture(menuTexture);
	menu->SetSprite(menuSprite);

	InitFont();
	InitHealthSprite();
	SetupLights();

	//Set music volume and play music
	gef::VolumeInfo volumeInfo;
	volumeInfo.volume = 0.2f;
	audio_manager_->SetMusicVolumeInfo(volumeInfo);

	audio_manager_->PlayMusic();
}

void SceneApp::CleanUp()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	delete backMesh2;
	backMesh2 = NULL;

	delete input_manager_;
	input_manager_ = NULL;

	CleanUpFont();

	delete gameManager;
	gameManager = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete player;
	player = NULL;

	// free up audio assets
	if (audio_manager_)
	{
		if (sfx_id_shoot != -1)
			audio_manager_->UnloadSample(sfx_id_shoot);
		if (sfx_id_move != -1)
			audio_manager_->UnloadSample(sfx_id_move);
		if (sfx_id_explode != -1)
			audio_manager_->UnloadSample(sfx_id_explode);
		audio_manager_->UnloadMusic();
	}

	delete audio_manager_;
	audio_manager_ = NULL;

}

bool SceneApp::Update(float frame_time)
{
	//Get frames per second
	fps_ = 1.0f / frame_time;

	//set player speed based on fps
	playerSpeed = 6 * 60.0f / fps_;

	//gravity strength based on fps
	gravityAmount = 9.81f * 60.0f / fps_;

	//fps scale for multiplying anything else with a velocity
	fpsScale = 60.0f / fps_;

	//update controls
	if (input_manager_)
	{
		input_manager_->Update();

		//ProcessKeyboardInput();
		ProcessControllerInput();
	}

	//Check which state game is in and update appropriately
	switch (gameManager->GetState())
	{
		case PAUSE:
			UpdatePause();
			break;
		case MENU:
			UpdateMenu();
			break;
		case PLAYING:
			UpdatePlaying(frame_time);
			break;
	}

	return true;
}

void SceneApp::UpdateMenu()
{

}

void SceneApp::UpdatePause()
{

}

void SceneApp::UpdatePlaying(float frame_time)
{
	// update physics world
	float32 timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update player with both stick values and current camera position
	player->Update(rightStickX, rightStickY, b2Vec2(camera.GetCameraUp().x(), camera.GetCameraUp().y()));

	//update gameplay values
	enemiesLeft = gameManager->GetEnemiesAlive();
	levelNo = gameManager->GetLevelNo();
	playerHealth = player->GetHealth();


	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	//loop over all contact points
	for (int contact_num = 0; contact_num<contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			//set empty bodies
			b2Body* playerBody = NULL;
			b2Body* enemyBody = NULL;
			b2Body* bulletBody = NULL;
			b2Body* bulletBody2 = NULL;
			b2Body* tileBody = NULL;

			//set empty gameobjects
			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			//get gameobjects from colliding bodies
			gameObjectA = (GameObject*)bodyA->GetUserData();
			gameObjectB = (GameObject*)bodyB->GetUserData();


			GameObject* playerTemp = NULL;
			GameObject* enemyTemp = NULL;
			GameObject* bulletTemp = NULL;
			GameObject* bulletTemp2 = NULL;
			GameObject* tileTemp = NULL;



			// figure which one is the player/bullet/enemy/tile
			if (gameObjectA != NULL && gameObjectA->GetType() == PLAYER)
			{
				playerTemp = gameObjectA;
				playerBody = bodyA;
			}
			else if (gameObjectA != NULL && gameObjectA->GetType() == BULLET)
			{
				bulletTemp = gameObjectA;
				bulletBody = bodyA;
			}
			else if (gameObjectA != NULL && gameObjectA->GetType() == ENEMY)
			{
				enemyTemp = gameObjectA;
				enemyBody = bodyA;
			}
			else if (gameObjectA != NULL && gameObjectA->GetType() == TILE)
			{
				tileTemp = gameObjectA;
				tileBody = bodyA;
			}

			if (gameObjectB != NULL && gameObjectB->GetType() == PLAYER)
			{
				playerTemp = gameObjectB;
				playerBody = bodyB;
			}
			else if (gameObjectB != NULL && gameObjectB->GetType() == BULLET)
			{
				//extra statement to check if 2 bullets are colliding, as one can be an explosion
				if (gameObjectA->GetType() == BULLET)
				{
					bulletTemp2 = gameObjectB;
					bulletBody2 = bodyB;
				}
				else
				{
					bulletTemp = gameObjectB;
					bulletBody = bodyB;
				}
			}
			else if (gameObjectB != NULL && gameObjectB->GetType() == ENEMY)
			{
				enemyTemp = gameObjectB;
				enemyBody = bodyB;
			}
			else if (gameObjectB != NULL && gameObjectB->GetType() == TILE)
			{
				tileTemp = gameObjectB;
				tileBody = bodyB;
			}

			//Collision responses for each possible collision
			if (playerTemp && bulletTemp)
			{
				//players are hurt by enemy bullets or any explosion
				if (bulletTemp->GetBulletType() == ENEMYBULLET || bulletTemp->GetBulletType() == EXPLOSION)
				{
					//reduve health and set the bullet to inactive
					player->ReduceHealth();
					bulletBody->SetActive(false);

					if (player->GetHealth() <= 0)
					{
						//reset game
						PlayerDeath();
					}
				}
				else
				{
					bulletBody->SetActive(false);
				}
				break;
			}
			//bullet hits enemy
			else if (enemyTemp && bulletTemp)
			{
				//Enemy is destroyed by either player bullet or an explosion
				if (bulletTemp->GetBulletType() == PLAYERBULLET || bulletTemp->GetBulletType() == EXPLOSION)
				{
					//Lower enemy count and set bullet to inactive
					gameManager->ReduceEnemyCount();
					enemyBody->SetActive(false);
					if (bulletTemp->GetBulletType() == PLAYERBULLET)
						bulletBody->SetActive(false);

					if (gameManager->GetEnemiesAlive() <= 0)
					{
						//Load next level if there are no enemies left
						ResetLevel(true);
					}
					break;
				}
				else
				{
					bulletBody->SetActive(false);
					break;
				}
			}
			//bullet collides with a tile
			else if (bulletTemp && tileTemp)
			{
				//Only set to inactive if it is not an explosion
				if (bulletTemp->GetBulletType() != EXPLOSION)
				{
					bulletBody->SetActive(false);
					break;
				}
			}
			//bulet collides with another bullet
			else if (bulletTemp && bulletTemp2)
			{
				//explosion destroys bullet, 2 normal bullet bounce off of each other
				if (bulletTemp->GetBulletType() == EXPLOSION && bulletTemp2->GetBulletType() != EXPLOSION)
				{
					bulletBody2->SetActive(false);
				}
				else if (bulletTemp2->GetBulletType() == EXPLOSION && bulletTemp->GetBulletType() != EXPLOSION)
				{
					bulletBody->SetActive(false);
				}
			}

		}

		// Get next contact point
		contact = contact->GetNext();
	}

	gameManager->Update(world_->GetGravity(), player->GetPosition(), fpsScale);
}

void SceneApp::ProcessControllerInput()
{
	const gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
	if (controller_input)
	{
		const gef::SonyController* controller = controller_input->GetController(0);
		if (controller)
		{
			//Update menu controls
			if (gameManager->GetState() == MENU)
			{
				if (controller->buttons_pressed() & gef_SONY_CTRL_START)
				{
					ResetLevel(false);
				}

				if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
				{
					gameManager->ChangeDifficulty();
				}
			}

			//Update pause screen controls
			else if (gameManager->GetState() == PAUSE)
			{
				if (controller->buttons_pressed() & gef_SONY_CTRL_START)
				{
					gameManager->SetState(PLAYING);
					return;
				}
			}

			//Update main game controls
			else
			{
				//Start button pauses game and stops the update
				if (controller->buttons_pressed() & gef_SONY_CTRL_START)
				{
					gameManager->SetState(PAUSE);
					return;
				}
				//Level skip for debugging
				if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
				{
					//ResetLevel(true);
				}
				//R2 fires or explodes current bullet
				if (controller->buttons_pressed() & gef_SONY_CTRL_R2)
				{
					//Check if the player can currently shoot
					if (player->GetCanShoot())
					{
						player->Shoot(fpsScale);
					}
					//Explode currently fired bullet if it exists
					else
					{
						player->ExplodeBullet();
					}
				}
				//Check if l2 has been pressed (for setting velocity later)
				if (controller->buttons_pressed() & gef_SONY_CTRL_L2)
				{
					L2Pressed = true;
				}

				//Check if the stick has been moved left or right
				if (controller->left_stick_x_axis() >= 0.3f)
				{
					RightPressed();

					//set l2pressed to false after attempting to move
					L2Pressed = false;
				}

				else if (controller->left_stick_x_axis() <= -0.3f)
				{
					LeftPressed();

					//set l2pressed to false after attempting to move
					L2Pressed = false;
				}

				//stop player and set player moving bools to false if left stick is not left or right
				else
				{
					//set horizontal velocity to zero based on current camera rotation
					if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
					{
						player->SetVelocity(b2Vec2(0.0f, player->GetVelocity().y));
					}
					else
					{
						player->SetVelocity(b2Vec2(player->GetVelocity().x, 0.0f));
					}
					//stop the annoying move sound
					player->SetMoveSound(false);

					//if camera is not currently rotating
					if (!camera.GetRotating())
					{
						player->SetPlayerRight(false);
						player->SetPlayerLeft(false);
					}
				}

				//leave right stick values as previous so the turret is always aiming somewhere
				if (controller->right_stick_x_axis() != 0 || controller->right_stick_y_axis() != 0)
				{
					rightStickX = controller->right_stick_x_axis();
					rightStickY = controller->right_stick_y_axis();
				}
			}
		}
	}
}

//keyboard controls for debugging without vita
//void SceneApp::ProcessKeyboardInput()
//{
//	const gef::Keyboard* keyboard = input_manager_->keyboard();
//	if (keyboard)
//	{
//		if (gameManager->GetState() == MENU)
//		{
//			if (keyboard->IsKeyDown(gef::Keyboard::KC_RETURN))
//			{
//				ResetLevel(false);
//			}
//			
//			if (keyboard->IsKeyDown(gef::Keyboard::KC_C))
//			{
//				gameManager->ChangeDifficulty();
//			}
//		}
//		else
//		{
//			
//			if (keyboard->IsKeyDown(gef::Keyboard::KC_RIGHT))
//			{
//				RightPressed();
//			}
//			else if (keyboard->IsKeyDown(gef::Keyboard::KC_LEFT))
//			{
//				LeftPressed();
//			}
//			else
//			{
//				StopPlayer();
//			}
//
//			if (keyboard->IsKeyDown(gef::Keyboard::KC_SPACE))
//			{
//				player->Shoot(fpsScale);
//			}
//			if (keyboard->IsKeyDown(gef::Keyboard::KC_P))
//			{
//				if (gameManager->GetState() == PLAYING)
//				{
//					ResetLevel(true);
//				}
//			}
//			if (keyboard->IsKeyDown(gef::Keyboard::KC_R))
//			{
//				if (gameManager->GetState() == PLAYING)
//				{
//					ResetLevel(false);
//				}
//			}
//		}
//	}
//}


void SceneApp::Render()
{
	//Render menu
	if (gameManager->GetState() == MENU)
	{
		//start and stop renderer as sprites dont seem to appear without it
		//renderer_3d_->Begin();

		//renderer_3d_->End();

		//begin sprite renderer and draw the menu screen
		sprite_renderer_->Begin(false);
		sprite_renderer_->DrawSprite(menu->GetSprite());
		DrawMenu();
		sprite_renderer_->End();
	}

	//Render pause screen
	else if (gameManager->GetState() == PAUSE)
	{
		//renderer_3d_->Begin();
		//
		//renderer_3d_->End();

		sprite_renderer_->Begin(false);
		sprite_renderer_->DrawSprite(pauseSprite);
		sprite_renderer_->End();
	}

	else
	{
		// setup camera

		// projection
		float fov = gef::DegToRad(45.0f);
		float aspect_ratio = (float)platform_.width() / (float)platform_.height();
		gef::Matrix44 projection_matrix;
		projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
		renderer_3d_->set_projection_matrix(projection_matrix);

		// set up view from camera class
		int cameraHeight = 8;
		//Keep camera looking at player
		gef::Vector4 camera_eye(player->GetPosition().x + (camera.GetCameraUp().x() * cameraHeight), player->GetPosition().y + (camera.GetCameraUp().y() * cameraHeight), 30.0f);
		gef::Vector4 camera_lookat(player->GetPosition().x + (camera.GetCameraUp().x() * cameraHeight), player->GetPosition().y + (camera.GetCameraUp().y() * cameraHeight), 0.0f);

		gef::Matrix44 view_matrix;
		view_matrix.LookAt(camera_eye, camera_lookat, camera.GetCameraUp());
		renderer_3d_->set_view_matrix(view_matrix);

		// draw 3d geometry
		renderer_3d_->Begin();

		//draw background mesh
		renderer_3d_->set_override_material(backMaterial);
		renderer_3d_->DrawMesh(back);
		renderer_3d_->set_override_material(NULL);

		//draw background mesh2
		renderer_3d_->set_override_material(blackMaterial);
		renderer_3d_->DrawMesh(back2);
		renderer_3d_->set_override_material(NULL);

		//draw all tiles from gamemanager current level
		renderer_3d_->set_override_material(tileMaterial);
		for (int i = 0; i < levelWidth; i++)
		{
			for (int j = 0; j < levelHeight; j++)
			{
				if (gameManager->GetBlock(i, j) != NULL)
					renderer_3d_->DrawMesh(*gameManager->GetBlock(i, j));
			}
		}
		renderer_3d_->set_override_material(NULL);

		// draw player
		renderer_3d_->set_override_material(playerMaterial);
		renderer_3d_->DrawMesh(*player);
		renderer_3d_->set_override_material(NULL);
		renderer_3d_->set_override_material(&primitive_builder_->orange_material());
		renderer_3d_->DrawMesh(*player->GetTurret());
		renderer_3d_->set_override_material(NULL);

		//draw all current enemies
		renderer_3d_->set_override_material(enemyMaterial);
		for (int i = 0; i < gameManager->GetEnemyCount(); i++)
		{
			if (!gameManager->GetEnemy(i)->GetDead())
			{
				renderer_3d_->DrawMesh(*gameManager->GetEnemy(i));
			}
		}
		renderer_3d_->set_override_material(NULL);

		//draw bullets if active
		renderer_3d_->set_override_material(&primitive_builder_->red_material());
		for (int i = 0; i < gameManager->GetEnemyCount(); i++)
		{
			if (!gameManager->GetEnemy(i)->GetCanShoot() && !gameManager->GetEnemy(i)->GetDead())
			{
				renderer_3d_->DrawMesh(*gameManager->GetEnemy(i)->GetBulletMesh());
			}
		}
		renderer_3d_->set_override_material(NULL);
		
		renderer_3d_->set_override_material(&primitive_builder_->green_material());
		if (!player->GetCanShoot())
		{
			renderer_3d_->DrawMesh(*player->GetBulletMesh());
		}
		renderer_3d_->set_override_material(NULL);

		//draw explosions
		renderer_3d_->set_override_material(explodeMaterial);
		if (player->IsExploding())
		{
			renderer_3d_->DrawMesh(*player->GetExplosionMesh());
		}
		renderer_3d_->set_override_material(NULL);
		renderer_3d_->End();

		// start drawing sprites, but don't clear the frame buffer
		sprite_renderer_->Begin(false);
		DrawHUD();
		sprite_renderer_->End();
	}
}

void SceneApp::DrawHUD()
{
	//draw health green squares
	for (int i = 0; i < player->GetHealth(); i++)
	{
		sprite_renderer_->DrawSprite(healths[i]);
	}
	if (font_)
	{
		// display frame rate
		//font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);

		//draw letters HEALTH on the health squares so people know thats the health
		font_->RenderText(sprite_renderer_, gef::Vector4(healths[0].position().x() - 10.0f, healths[0].position().y() - 15.0f, -1.0f), 1.0f, 0xff000000, gef::TJ_LEFT, "H");
		font_->RenderText(sprite_renderer_, gef::Vector4(healths[1].position().x() - 10.0f, healths[1].position().y() - 15.0f, -1.0f), 1.0f, 0xff000000, gef::TJ_LEFT, "E");
		font_->RenderText(sprite_renderer_, gef::Vector4(healths[2].position().x() - 10.0f, healths[2].position().y() - 15.0f, -1.0f), 1.0f, 0xff000000, gef::TJ_LEFT, "A");
		font_->RenderText(sprite_renderer_, gef::Vector4(healths[3].position().x() - 10.0f, healths[3].position().y() - 15.0f, -1.0f), 1.0f, 0xff000000, gef::TJ_LEFT, "L");
		font_->RenderText(sprite_renderer_, gef::Vector4(healths[4].position().x() - 10.0f, healths[4].position().y() - 15.0f, -1.0f), 1.0f, 0xff000000, gef::TJ_LEFT, "T");
		font_->RenderText(sprite_renderer_, gef::Vector4(healths[5].position().x() - 10.0f, healths[5].position().y() - 15.0f, -1.0f), 1.0f, 0xff000000, gef::TJ_LEFT, "H");

		//display enemies remaining and current level number
		font_->RenderText(sprite_renderer_, gef::Vector4(15.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Enemies left: %i", enemiesLeft);
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 15.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "Level  %i", levelNo);
	}
	
}

void SceneApp::DrawMenu()
{
	//for displaying currently difficulty setting
	char* difficulty;
	switch (gameManager->GetDifficulty())
	{
		case 1:
			difficulty = "easy";
			break;
		case 2:
			difficulty = "normal";
			break;
		case 3:
			difficulty = "hard";
		break;
	}
	if (font_)
	{
		font_->RenderText(sprite_renderer_, gef::Vector4(320.0f, 305.0f, -0.9f), 2.0f, 0xff000000, gef::TJ_LEFT, "Begin");
		font_->RenderText(sprite_renderer_, gef::Vector4(320.0f, 405.0f, -0.9f), 2.0f, 0xff000000, gef::TJ_LEFT, "Difficulty : %s", difficulty);
	}
}

void SceneApp::InitBackground()
{
	// background dimensions
	gef::Vector4 back_half_dimensions(30.0f, 30.0f, 0.1f);
	gef::Vector4 back_half_dimensions2(60.0f, 60.0f, 0.1f);

	// setup the mesh for the backgrounds
	backMesh = primitive_builder_->CreateBoxMesh(back_half_dimensions);
	back.set_mesh(backMesh);

	backMesh2 = primitive_builder_->CreateBoxMesh(back_half_dimensions2);
	back2.set_mesh(backMesh2);
	
	//setup the background rotations
	gef::Matrix44 object_rotation;
	object_rotation.RotationZ(0);

	// setup the background translations
	gef::Vector4 object_translation(30.0f, -30.0f, -1.0f);
	gef::Vector4 object_translation2(30.0f, -30.0f, -2.0f);

	// build background transformation matrix
	gef::Matrix44 object_transform = object_rotation;
	object_transform.SetTranslation(object_translation);
	back.set_transform(object_transform);
	object_transform.SetTranslation(object_translation2);
	back2.set_transform(object_transform);
}


void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::InitHealthSprite()
{
	//setup player health sprites
	for (int i = 0; i < player->GetMaxHealth(); i++)
	{
		healths[i].set_position(gef::Vector4((40 * i) + 30, 30, -0.5f));
		healths[i].set_height(30);
		healths[i].set_width(30);
		healths[i].set_colour(0xFF00FF00);
	}
	
}

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}



void SceneApp::SetupLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void SceneApp::RightPressed()
{
	//If camera is currently rotating
	if (camera.GetRotating())
	{
		//set player pushed right this frame
		player->SetPlayerRight(true);
		//if player was previously previously holding left change the rotation target
		if (player->GetPlayerLeft())
		{
			camera.ChangeCameraTarget(-1);
			player->SetPlayerLeft(false);
		}
		//if camera has fully rotated stop and set so player wont rotate again until moved
		if (camera.CameraTargetReached())
		{
			camera.SetRotating(false);
			player->SetPlayerRight(false);
		}
		//continue to rotate the camera and update gravity
		else
		{
			camera.RotateCam(-1);
			world_->SetGravity(b2Vec2(-gravityAmount * camera.GetCameraUp().x(), -gravityAmount * camera.GetCameraUp().y()));
		}
	}
	//if player is holding right but not moving horizontally then start rotating the camera and stop the player
	else if (b2Abs(player->GetVelocity().x) < 0.01f && b2Abs(player->GetVelocity().y) < 0.01f && player->GetPlayerRight())
	{
		//rotate
		camera.SetRotating(true);
		camera.ChangeCameraTarget(-1);
		player->SetVelocity(b2Vec2_zero);
	}
	//if nothing else is happening then move the player
	else
	{
		//if camera is at position 0 or 2 then set x velocity, unless L2 was pressed
		if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
		{
			if (L2Pressed)
				player->SetVelocity(b2Vec2(0, player->GetVelocity().y));
			else
				player->SetVelocity(b2Vec2(playerSpeed * camera.GetCameraRight().x, player->GetVelocity().y));
		}
		//if camera is at position 1 or 3 then set y velocity, unless L2 was pressed
		else
		{
			if(L2Pressed)
				player->SetVelocity(b2Vec2(player->GetVelocity().x, 0));
			else
				player->SetVelocity(b2Vec2(player->GetVelocity().x, playerSpeed * camera.GetCameraRight().y));
		}

		//Set that player pressed right this frame
		player->SetPlayerRight(true);
		//moving sound
		player->SetMoveSound(true);
	}

	//say that player wasnt moving left this frame
	player->SetPlayerLeft(false);


}
void SceneApp::LeftPressed()
{
	//If camera is currently rotating
	if (camera.GetRotating())
	{
		//set player pushed left this frame
		player->SetPlayerLeft(true);
		//if player was previously previously holding right change the rotation target
		if (player->GetPlayerRight())
		{
			camera.ChangeCameraTarget(1);
			player->SetPlayerRight(false);
		}
		//if camera has fully rotated stop and set so player wont rotate again until moved
		if (camera.CameraTargetReached())
		{
			camera.SetRotating(false);
			player->SetPlayerLeft(false);
		}
		//continue to rotate the camera and update gravity
		else
		{
			camera.RotateCam(1);
			world_->SetGravity(b2Vec2(-gravityAmount * camera.GetCameraUp().x(), -gravityAmount * camera.GetCameraUp().y()));
		}
	}
	//if player is holding left but not moving horizontally then start rotating the camera and stop the player
	else if (b2Abs(player->GetVelocity().x) < 0.01f && b2Abs(player->GetVelocity().y) < 0.01f && player->GetPlayerLeft())
	{
		//rotate
		camera.SetRotating(true);
		camera.ChangeCameraTarget(1);
		player->SetVelocity(b2Vec2_zero);
	}
	//if nothing else is happening then move the player
	else
	{
		//if camera is at position 0 or 2 then set x velocity, unless L2 was pressed
		if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
		{
			if (L2Pressed)
				player->SetVelocity(b2Vec2(0, player->GetVelocity().y));
			else
				player->SetVelocity(b2Vec2(-playerSpeed * camera.GetCameraRight().x, player->GetVelocity().y));
		}
		//if camera is at position 1 or 3 then set y velocity, unless L2 was pressed
		else
		{
			if (L2Pressed)
				player->SetVelocity(b2Vec2(player->GetVelocity().x, 0));
			else
				player->SetVelocity(b2Vec2(player->GetVelocity().x, -playerSpeed * camera.GetCameraRight().y));
		}
		//Set that player pressed right this frame
		player->SetPlayerLeft(true);
		//moving sound
		player->SetMoveSound(true);
	}
	//say that player wasnt moving left this frame
	player->SetPlayerRight(false);
}



void SceneApp::ResetLevel(bool nextLevel)
{
	//Set L2 to not pressed
	L2Pressed = false;
	//change game state to playing
	gameManager->SetState(PLAYING);
	//remove everything currently in the level
	gameManager->Reset();
	//add one to current level count or return to menu if last level
	if (nextLevel)
	{
		gameManager->NextLevel();
		if (gameManager->GetLevelNo() == 1)
		{
			gameManager->SetState(MENU);
		}
	}
	//load level
	gameManager->LoadLevel();
	//set enemy count
	enemiesLeft = gameManager->GetEnemiesAlive();
	//set level number text
	levelNo = gameManager->GetLevelNo();
	//reset camera to start position
	camera.ResetCamera();
	//reset gravity
	world_->SetGravity(b2Vec2(0, -gravityAmount));
	//reset player values
	player->ResetPlayer(gameManager->GetStartPosition());
	player->SetHealth(playerHealth);
}

void SceneApp::PlayerDeath()
{
	//set players health back to max
	player->SetHealth(player->GetMaxHealth());
	playerHealth = player->GetHealth();
	//reload level
	ResetLevel(false);
	//reset back to menu
	gameManager->ResetAll();
}

void SceneApp::InitTextures()
{
	//Initialise textures, materials and sprites
	pngLoader.Load("player.png", platform_, playerImage);
	playerTexture = gef::Texture::Create(platform_, playerImage);
	playerMaterial = new gef::Material();
	//playerMaterial.set_colour(0xff0000ff);
	playerMaterial->set_texture(playerTexture);

	pngLoader.Load("enemy.png", platform_, enemyImage);
	enemyTexture = gef::Texture::Create(platform_, enemyImage);
	enemyMaterial = new gef::Material();
	//enemyMaterial.set_colour(0xff0000ff);
	enemyMaterial->set_texture(enemyTexture);

	pngLoader.Load("explode.png", platform_, explodeImage);
	explodeTexture = gef::Texture::Create(platform_, explodeImage);
	explodeMaterial = new gef::Material();
	//enemyMaterial.set_colour(0xff0000ff);
	explodeMaterial->set_texture(explodeTexture);


	pngLoader.Load("tile.png", platform_, tileImage);
	tileTexture = gef::Texture::Create(platform_, tileImage);
	tileMaterial = new gef::Material();
	//tileMaterial.set_colour(0xff0000ff);
	tileMaterial->set_texture(tileTexture);

	pngLoader.Load("background.png", platform_, backImage);
	backTexture = gef::Texture::Create(platform_, backImage);
	backMaterial = new gef::Material();
	//mat.set_colour(0xff0000ff);
	backMaterial->set_texture(backTexture);

	pngLoader.Load("black.png", platform_, blackImage);
	blackTexture = gef::Texture::Create(platform_, blackImage);
	blackMaterial = new gef::Material();
	//mat.set_colour(0xff0000ff);
	blackMaterial->set_texture(blackTexture);


	pngLoader.Load("menuBack.png", platform_, menuImage);
	menuTexture = gef::Texture::Create(platform_, menuImage);
	menuMaterial = new gef::Material();
	//mat.set_colour(0xff0000ff);
	menuMaterial->set_texture(menuTexture);

	pngLoader.Load("pause.png", platform_, pauseImage);
	pauseTexture = gef::Texture::Create(platform_, pauseImage);
	//mat.set_colour(0xff0000ff);
	pauseSprite.set_texture(pauseTexture);
	pauseSprite.set_height(544);
	pauseSprite.set_width(960);
	pauseSprite.set_position(gef::Vector4(480, 272, 0));
}

