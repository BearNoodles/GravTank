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
	toDel(NULL),
	audio_manager_(NULL),
	sfx_id_(-1),
	sfx_id_2(-1)
{
}

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	input_manager_ = platform_.CreateInputManager();

	audio_manager_ = gef::AudioManager::Create();

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	pngLoader.Load("unwhite.png", platform_, img);
	tex = gef::Texture::Create(platform_, img);
	//tex->Create(platform_, img);

	mat = new gef::Material();
	//mat.set_colour(0xff0000ff);
	mat->set_texture(tex);

	pngLoader.Load("fac.png", platform_, playerImage);
	playerTexture = gef::Texture::Create(platform_, playerImage);
	playerMaterial = new gef::Material();
	//playerMaterial.set_colour(0xff0000ff);
	playerMaterial->set_texture(playerTexture);

	pngLoader.Load("enemy.png", platform_, enemyImage);
	enemyTexture = gef::Texture::Create(platform_, enemyImage);
	enemyMaterial = new gef::Material();
	//enemyMaterial.set_colour(0xff0000ff);
	enemyMaterial->set_texture(enemyTexture);

	pngLoader.Load("tile.png", platform_, tileImage);
	tileTexture = gef::Texture::Create(platform_, tileImage);
	tileMaterial = new gef::Material();
	//tileMaterial.set_colour(0xff0000ff);
	tileMaterial->set_texture(tileTexture);

	rightStickX = 0;
	rightStickY = -1;

	screenHeight = 544;
	screenWidth = 960;

	if (audio_manager_)
	{
		sfx_id_ = audio_manager_->LoadSample("pew.wav", platform_);
		sfx_id_2 = audio_manager_->LoadSample("print.wav", platform_);
		audio_manager_->LoadMusic("music.wav", platform_);
	}

	// initialise the physics world
	gravityAmount = 9.81f;
	b2Vec2 gravity(0.0f, -gravityAmount);
	world_ = new b2World(gravity);
	gameManager = new GameManager(world_, primitive_builder_, audio_manager_, sfx_id_, sfx_id_2);
	gameManager->LoadLevel();
	player = new Player(world_, primitive_builder_, gameManager->GetStartPosition(), audio_manager_, sfx_id_, sfx_id_2);
	playerSpeed = 6;
	InitGround();
	InitBuildings();

	pngLoader.Load("background.png", platform_, backImage);
	backTexture = gef::Texture::Create(platform_, backImage);
	backMaterial = new gef::Material();
	//mat.set_colour(0xff0000ff);
	backMaterial->set_texture(backTexture);
	backSprite.set_texture(backTexture);
	backSprite.set_height(60);
	backSprite.set_width(60);
	backSprite.set_position(gef::Vector4(0, 0, 1));
	

	pngLoader.Load("menuBack.png", platform_, menuImage);
	menuTexture = gef::Texture::Create(platform_, menuImage);
	menuMaterial = new gef::Material();
	//mat.set_colour(0xff0000ff);
	menuMaterial->set_texture(menuTexture);

	menu = new Menu(screenWidth, screenHeight);
	
	menuSprite.set_texture(menuTexture);
	menu->SetSprite(menuSprite);

	InitFont();
	InitHealthSprite();
	SetupLights();

	//gef::VolumeInfo volumeInfo;
	//volumeInfo.volume = 0.2f;
	//audio_manager_->SetMusicVolumeInfo(volumeInfo);

	//audio_manager_->PlayMusic();
}

void SceneApp::CleanUp()
{
	// destroying the physics world also destroys all the objects within it
	delete world_;
	world_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	delete buildingMesh;
	buildingMesh = NULL;

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
		if (sfx_id_ != -1)
			audio_manager_->UnloadSample(sfx_id_);
		if (sfx_id_2 != -1)
			audio_manager_->UnloadSample(sfx_id_2);
		audio_manager_->UnloadMusic();
	}

	delete audio_manager_;
	audio_manager_ = NULL;

}

bool SceneApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	playerSpeed = 6 * 60.0f / fps_;

	gravityAmount = 9.81f * 60.0f / fps_;

	if (input_manager_)
	{
		input_manager_->Update();

		ProcessKeyboardInput();
		//ProcessControllerInput();
	}

	switch (gameManager->GetState())
	{
		case LOADING:
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

void SceneApp::UpdatePlaying(float frame_time)
{
	// update physics world
	float32 timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update object visuals from simulation data
	player->Update(rightStickX, rightStickY, frame_time);

	

	// don't have to update the ground visuals as it is static


	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();


	for (int contact_num = 0; contact_num<contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			b2Body* playerBody = NULL;
			b2Body* enemyBody = NULL;
			b2Body* bulletBody = NULL;
			b2Body* tileBody = NULL;


			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = (GameObject*)bodyA->GetUserData();
			gameObjectB = (GameObject*)bodyB->GetUserData();

			GameObject* playerTemp = NULL;
			GameObject* enemyTemp = NULL;
			GameObject* bulletTemp = NULL;
			GameObject* tileTemp = NULL;



			// figure which one is the player
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
				bulletTemp = gameObjectB;
				bulletBody = bodyB;
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

			if (playerTemp && bulletTemp)
			{
				if (bulletTemp->GetBulletType() == ENEMYBULLET)
				{
					player->ReduceHealth();
					bulletBody->SetActive(false);

					if (player->GetHealth() <= 0)
					{
						gameManager->Reset();
						gameManager->LoadLevel();
						player->ResetPlayer(gameManager->GetStartPosition());
					}
				}
				else
				{
					bulletBody->SetActive(false);
				}
				break;
			}
			else if (enemyTemp && bulletTemp)
			{
				if (bulletTemp->GetBulletType() == PLAYERBULLET)
				{
					gameManager->ReduceEnemyCount();
					enemyBody->SetActive(false);
					bulletBody->SetActive(false);

					if (gameManager->GetEnemiesAlive() <= 0)
					{
						gameManager->Reset();
						gameManager->NextLevel();
						gameManager->LoadLevel();
						player->ResetPlayer(gameManager->GetStartPosition());
					}
					break;
				}
				else
				{
					bulletBody->SetActive(false);
					break;
				}
			}
			else if (bulletTemp && tileTemp)
			{
				bulletBody->SetActive(false);
				break;
			}

		}

		// Get next contact point
		contact = contact->GetNext();
	}

	gameManager->Update(world_->GetGravity());
}

void SceneApp::ProcessControllerInput()
{
	const gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
	if (controller_input)
	{
		const gef::SonyController* controller = controller_input->GetController(0);
		if (controller)
		{
			if (gameManager->GetState() == MENU)
			{
				if (controller->buttons_pressed() & gef_SONY_CTRL_START)
				{
					gameManager->SetState(PLAYING);
					gameManager->Reset();
					gameManager->LoadLevel();
					player->ResetPlayer(gameManager->GetStartPosition());
				}

				if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
				{
					gameManager->ChangeDifficulty();
				}
			}

			// check for button presses and print out some debug text
			if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
				gef::DebugOut("CROSS pressed\n");
			if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE)
				gef::DebugOut("SQUARE pressed\n");
			if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
			{
				gef::DebugOut("TRIANGLE pressed\n");
				gameManager->Reset();
				gameManager->NextLevel();
				gameManager->LoadLevel();
				player->ResetPlayer(gameManager->GetStartPosition());
			}
			if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
				gef::DebugOut("CIRCLE pressed\n");
			if (controller->buttons_pressed() & gef_SONY_CTRL_R2)
			{
				b2Vec2 shotForce(rightStickX, -rightStickY);
				shotForce.Normalize();
				shotForce = b2Vec2(shotForce.x * 200, shotForce.y * 200);
				player->Shoot(shotForce);
			}
			if (controller->buttons_pressed() & gef_SONY_CTRL_L2)
				player->Shoot(b2Vec2(0, 100));

			if (controller->left_stick_x_axis() >= 0.3f)
			{
				RightPressed();
				player->SetMoveSound(true);
			}

			else if (controller->left_stick_x_axis() <= -0.3f)
			{
				LeftPressed();
				player->SetMoveSound(true);
			}

			else
			{
				if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
				{
					player->SetVelocity(b2Vec2(0.0f, player->GetVelocity().y));
				}
				else
				{
					player->SetVelocity(b2Vec2(player->GetVelocity().x, 0.0f));
				}
				player->SetMoveSound(false);

				if (!camera.GetRotating())
				{
					player->SetPlayerRight(false);
					player->SetPlayerLeft(false);
				}
			}


			// print out some debug text when left stick is moved
			if (controller->right_stick_x_axis() != 0.0f)
				gef::DebugOut("right_stick_x_axis: %f\n", controller->right_stick_x_axis());
			if (controller->right_stick_y_axis() != 0.0f)
				gef::DebugOut("right_stick_y_axis: %f\n", controller->right_stick_y_axis());
			if (controller->right_stick_x_axis() != 0 || controller->right_stick_y_axis() != 0)
			{
				rightStickX = controller->right_stick_x_axis();
				rightStickY = controller->right_stick_y_axis();
			}
		}
	}
}

void SceneApp::ProcessKeyboardInput()
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{
		if (gameManager->GetState() == MENU)
		{
			if (keyboard->IsKeyDown(gef::Keyboard::KC_RETURN))
			{
				gameManager->SetState(PLAYING);
				gameManager->Reset();
				gameManager->LoadLevel();
				player->ResetPlayer(gameManager->GetStartPosition());
			}
			
			if (keyboard->IsKeyDown(gef::Keyboard::KC_C))
			{
				gameManager->ChangeDifficulty();
			}
		}
		else
		{
			
			if (keyboard->IsKeyDown(gef::Keyboard::KC_RIGHT))
			{
				RightPressed();
			}
			else if (keyboard->IsKeyDown(gef::Keyboard::KC_LEFT))
			{
				LeftPressed();
			}
			else
			{
				StopPlayer();
			}

			if (keyboard->IsKeyDown(gef::Keyboard::KC_SPACE))
			{
				player->Shoot(b2Vec2(0, 100));
			}
			if (keyboard->IsKeyDown(gef::Keyboard::KC_P))
			{
				if (gameManager->GetState() == PLAYING)
				{
					//gameManager->SetState(LOADING);
					gameManager->Reset();
					gameManager->NextLevel();
					gameManager->LoadLevel();
					player->ResetPlayer(gameManager->GetStartPosition());
				}
			}
			if (keyboard->IsKeyDown(gef::Keyboard::KC_R))
			{
				if (gameManager->GetState() == PLAYING)
				{
					//gameManager->SetState(LOADING);
					gameManager->Reset();
					gameManager->LoadLevel();
					player->SetPosition(gameManager->GetStartPosition());
				}
			}
		}
	}
}


void SceneApp::Render()
{
	if (gameManager->GetState() == MENU)
	{
		renderer_3d_->Begin();

		renderer_3d_->End();

		sprite_renderer_->Begin(false);
		sprite_renderer_->DrawSprite(menu->GetSprite());
		DrawMenu();
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

		// view
		int cameraHeight = 8;
		gef::Vector4 camera_eye(player->GetPosition().x + (camera.GetCameraUp().x() * cameraHeight), player->GetPosition().y + (camera.GetCameraUp().y() * cameraHeight), 30.0f);
		gef::Vector4 camera_lookat(player->GetPosition().x + (camera.GetCameraUp().x() * cameraHeight), player->GetPosition().y + (camera.GetCameraUp().y() * cameraHeight), 0.0f);

		gef::Matrix44 view_matrix;
		view_matrix.LookAt(camera_eye, camera_lookat, camera.GetCameraUp());
		renderer_3d_->set_view_matrix(view_matrix);

		// draw 3d geometry
		renderer_3d_->Begin();

		// draw ground
		renderer_3d_->DrawMesh(ground_);

		//draw building
		renderer_3d_->DrawMesh(building);
		renderer_3d_->DrawMesh(building2);

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
		renderer_3d_->set_override_material(NULL);
		renderer_3d_->DrawMesh(*player);
		renderer_3d_->DrawMesh(*player->GetTurret());
		renderer_3d_->set_override_material(NULL);

		//rendenemy
		renderer_3d_->set_override_material(enemyMaterial);
		for (int i = 0; i < gameManager->GetEnemyCount(); i++)
		{
			if (!gameManager->GetEnemy(i)->GetDead())
			{
				renderer_3d_->DrawMesh(*gameManager->GetEnemy(i));
			}
		}
		renderer_3d_->set_override_material(NULL);

		//
		renderer_3d_->set_override_material(&primitive_builder_->blue_material());
		for (int i = 0; i < gameManager->GetEnemyCount(); i++)
		{
			if (!gameManager->GetEnemy(i)->GetCanShoot())
			{
				renderer_3d_->DrawMesh(*gameManager->GetEnemy(i)->GetBulletMesh());
			}
		}

		if (!player->GetCanShoot())
		{
			renderer_3d_->DrawMesh(*player->GetBulletMesh());
		}
		renderer_3d_->set_override_material(NULL);

		renderer_3d_->End();

		// start drawing sprites, but don't clear the frame buffer
		sprite_renderer_->Begin(false);
		DrawBack();
		DrawHUD();
		sprite_renderer_->End();
	}
}

void SceneApp::DrawHUD()
{
	if (font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
	for (int i = 0; i < player->GetHealth(); i++)
	{
		sprite_renderer_->DrawSprite(healths[i]);
	}
}

void SceneApp::DrawMenu()
{
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

void SceneApp::DrawBack()
{
	sprite_renderer_->DrawSprite(backSprite);
}



void SceneApp::InitGround()
{
	// ground dimensions
	gef::Vector4 ground_half_dimensions(50.0f, 0.5f, 0.5f);

	// setup the mesh for the ground
	ground_mesh_ = primitive_builder_->CreateBoxMesh(ground_half_dimensions);
	ground_.set_mesh(ground_mesh_);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(0.0f, 0.0f);

	ground_body_ = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixture on the rigid body
	ground_body_->CreateFixture(&fixture_def);

	// update visuals from simulation data
	ground_.UpdateFromSimulation(ground_body_);
}

void SceneApp::InitBuildings()
{
	// building dimensions
	gef::Vector4 building_half_dimensions(1.0f, 1.0f, 0.5f);

	// setup the mesh for the buildings
	buildingMesh = primitive_builder_->CreateBoxMesh(building_half_dimensions);
	building.set_mesh(buildingMesh);
	building2.set_mesh(buildingMesh);

	// create physics bodies
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	body_def.position = b2Vec2(4.0f, 1.0f);

	buildingBody = world_->CreateBody(&body_def);
	body_def.position = b2Vec2(-4.0f, 1.0f);
	buildingBody2 = world_->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(building_half_dimensions.x(), building_half_dimensions.y());

	b2PolygonShape shape2;
	shape2.SetAsBox(building_half_dimensions.x(), building_half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;

	// create the fixtures on the rigid bodies
	buildingBody->CreateFixture(&fixture_def);

	buildingBody2->CreateFixture(&fixture_def);

	//levelBuilder.LoadLevel(&platform_, *world_);

	// update visuals from simulation data
	building.UpdateFromSimulation(buildingBody);
	building2.UpdateFromSimulation(buildingBody2);
}


void SceneApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void SceneApp::InitHealthSprite()
{
	for (int i = 0; i < player->GetMaxHealth(); i++)
	{
		healths[i].set_position(gef::Vector4((40 * i) + 30, 30, 0));
		healths[i].set_height(30);
		healths[i].set_width(30);
		healths[i].set_colour(0xFF0000FF);
		healths[i].set_texture(tex);
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
	if (camera.GetRotating())
	{
		player->SetPlayerRight(true);
		if (player->GetPlayerLeft())
		{
			camera.ChangeCameraTarget(-1);
			player->SetPlayerLeft(false);
		}
		if (camera.CameraTargetReached())
		{
			camera.SetRotating(false);
			player->SetPlayerRight(false);
		}
		else
		{
			camera.RotateCam(-1);
			world_->SetGravity(b2Vec2(-gravityAmount * camera.GetCameraUp().x(), -gravityAmount * camera.GetCameraUp().y()));
		}
	}
	else if (b2Abs(player->GetVelocity().x) < 0.01f && b2Abs(player->GetVelocity().y) < 0.01f && player->GetPlayerRight())
	{
		//rotate
		camera.SetRotating(true);
		camera.ChangeCameraTarget(-1);
		player->SetVelocity(b2Vec2_zero);
	}

	else
	{
		if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
		{
			player->SetVelocity(b2Vec2(playerSpeed * camera.GetCameraRight().x, player->GetVelocity().y));
		}
		else
		{
			player->SetVelocity(b2Vec2(player->GetVelocity().x, playerSpeed * camera.GetCameraRight().y));
		}

		player->SetPlayerRight(true);
	}

	player->SetPlayerLeft(false);


}
void SceneApp::LeftPressed()
{
	if (camera.GetRotating())
	{
		player->SetPlayerLeft(true);
		if (player->GetPlayerRight())
		{
			camera.ChangeCameraTarget(1);
			player->SetPlayerRight(false);
		}
		if (camera.CameraTargetReached())
		{
			camera.SetRotating(false);
			player->SetPlayerLeft(false);
		}
		else
		{
			camera.RotateCam(1);
			world_->SetGravity(b2Vec2(-gravityAmount * camera.GetCameraUp().x(), -gravityAmount * camera.GetCameraUp().y()));
		}
	}
	else if (b2Abs(player->GetVelocity().x) < 0.01f && b2Abs(player->GetVelocity().y) < 0.01f && player->GetPlayerLeft())
	{
		//rotate
		camera.SetRotating(true);
		camera.ChangeCameraTarget(1);
		player->SetVelocity(b2Vec2_zero);
	}

	else
	{
		if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
		{
			player->SetVelocity(b2Vec2(-playerSpeed * camera.GetCameraRight().x, player->GetVelocity().y));
		}
		else
		{
			player->SetVelocity(b2Vec2(player->GetVelocity().x, -playerSpeed * camera.GetCameraRight().y));
		}
		player->SetPlayerLeft(true);
	}

	player->SetPlayerRight(false);
}

void SceneApp::StopPlayer()
{
	if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
	{
		player->SetVelocity(b2Vec2(0.0f, player->GetVelocity().y));
	}
	else
	{
		player->SetVelocity(b2Vec2(player->GetVelocity().x, 0.0f));
	}


	if (!camera.GetRotating())
	{
		player->SetPlayerRight(false);
		player->SetPlayerLeft(false);
	}
	
}

