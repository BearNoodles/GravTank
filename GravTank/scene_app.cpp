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
	player_body_(NULL),
	toDel(NULL)
{
}

static const char* key_names[] =
{
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"NUMPADENTER",
	"NUMPADSTAR",
	"NUMPADEQUALS",
	"NUMPADMINUS",
	"NUMPADPLUS",
	"NUMPADPERIOD",
	"NUMPADSLASH",
	"ESCAPE",
	"TAB",
	"LSHIFT",
	"RSHIFT",
	"LCONTROL",
	"RCONTROL",
	"BACKSPACE",
	"RETURN",
	"LALT",
	"SPACE",
	"CAPSLOCK",
	"NUMLOCK",
	"SCROLL",
	"RALT",
	"AT",
	"COLON",
	"UNDERLINE",
	"MINUS",
	"EQUALS",
	"LBRACKET",
	"RBRACKET",
	"SEMICOLON",
	"APOSTROPHE",
	"GRAVE",
	"BACKSLASH",
	"COMMA",
	"PERIOD",
	"SLASH",
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT"
};

void SceneApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	input_manager_ = platform_.CreateInputManager();

	// create the renderer for draw 3D geometry
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// initialise primitive builder to make create some 3D geometry easier
	primitive_builder_ = new PrimitiveBuilder(platform_);

	

	InitFont();
	SetupLights();

	// initialise the physics world
	gravityAmount = 9.81f;
	b2Vec2 gravity(0.0f, -gravityAmount);
	world_ = new b2World(gravity);
	levelBuilder = new LevelBuilder(world_, primitive_builder_);
	levelBuilder->LoadLevel();
	enemy = new Enemy(0, world_, primitive_builder_);
	InitPlayer();
	InitGround();
	InitBuildings();
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

	delete levelBuilder;
	levelBuilder = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	if (input_manager_)
	{
		input_manager_->Update();

		ProcessKeyboardInput();
		ProcessControllerInput();
	}

	// update physics world
	float32 timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// update object visuals from simulation data
	player_.UpdateFromSimulation(player_body_);

	enemy->Update(world_->GetGravity());

	// don't have to update the ground visuals as it is static


	// collision detection
	// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	// get contact count
	int contact_count = world_->GetContactCount();

	if (player_body_->GetContactList() == NULL)
	{
		camera.SetRotating(false);
	}
	

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


			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = (GameObject*)bodyA->GetUserData();
			gameObjectB = (GameObject*)bodyB->GetUserData();

			GameObject* playerTemp = NULL;
			GameObject* enemyTemp = NULL;

			//if (gameObjectA != NULL && gameObjectA->GetType() == BULLET)
			//{
			//	if (gameObjectB != NULL && gameObjectB->GetType() == BULLET)
			//	{
			//		continue;
			//	}
			//	else if (gameObjectB == NULL)
			//	{
			//		continue;
			//	}
			//	
			//	break;
			//}
			//else if (gameObjectB != NULL && gameObjectB->GetType() == BULLET)
			//{
			//	if (gameObjectA != NULL && gameObjectA->GetType() == BULLET)
			//	{
			//		continue;
			//	}
			//	else if (gameObjectA == NULL)
			//	{
			//		continue;
			//	}
			//	toDel = gameObjectB;
			//
			//	break;
			//}

			// figure which one is the player
			if (gameObjectA != NULL && gameObjectA->GetType() == PLAYER)
			{
				playerTemp = gameObjectA;
				playerBody = bodyA;
			}
			else if (gameObjectA != NULL && gameObjectA->GetType() == ENEMY)
			{
				enemyTemp = gameObjectA;
				enemyBody = bodyA;
			}

			if (gameObjectB != NULL && gameObjectB->GetType() == PLAYER)
			{
				playerTemp = gameObjectB;
				playerBody = bodyB;
			}
			else if (gameObjectB != NULL && gameObjectB->GetType() == ENEMY)
			{
				enemyTemp = gameObjectB;
				enemyBody = bodyB;
			}

			if (playerTemp)
			{
				playerTemp->MyCollisionResponse();
				//playerBody->ApplyForceToCenter(b2Vec2(100.0f, 200.0f), true);
			}
			if (enemyTemp)
			{
				enemyTemp->MyCollisionResponse();
			}

		}

		// Get next contact point
		contact = contact->GetNext();
	}

	return true;
}

void SceneApp::ProcessControllerInput()
{
	const gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
	if (controller_input)
	{
		const gef::SonyController* controller = controller_input->GetController(0);
		if (controller)
		{
			// check for button presses and print out some debug text
			if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
				gef::DebugOut("CROSS pressed\n");
			if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE)
				gef::DebugOut("SQUARE pressed\n");
			if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
				gef::DebugOut("TRIANGLE pressed\n");
			if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
				gef::DebugOut("CIRCLE pressed\n");

			// print out some debug text when left stick is moved
			if (controller->left_stick_x_axis() != 0.0f)
				gef::DebugOut("left_stick_x_axis: %f\n", controller->left_stick_x_axis());
			if (controller->left_stick_y_axis() != 0.0f)
				gef::DebugOut("left_stick_y_axis: %f\n", controller->left_stick_y_axis());

			// print out some debug text when left stick is moved
			if (controller->right_stick_x_axis() != 0.0f)
				gef::DebugOut("right_stick_x_axis: %f\n", controller->right_stick_x_axis());
			if (controller->right_stick_y_axis() != 0.0f)
				gef::DebugOut("right_stick_y_axis: %f\n", controller->right_stick_y_axis());
		}
	}
}

void SceneApp::ProcessKeyboardInput()
{
	const gef::Keyboard* keyboard = input_manager_->keyboard();
	if (keyboard)
	{
		// go through all the keys and print out some debug text when a key is pressed or released
		for (int key_num = 0; key_num < gef::Keyboard::NUM_KEY_CODES; ++key_num)
		{
			if (keyboard->IsKeyPressed((gef::Keyboard::KeyCode)key_num))
				gef::DebugOut("%s pressed\n", key_names[key_num]);
			else if (keyboard->IsKeyReleased((gef::Keyboard::KeyCode)key_num))
				gef::DebugOut("%s released\n", key_names[key_num]);
		}
		if (keyboard->IsKeyDown(gef::Keyboard::KC_RIGHT))
		{
			if (camera.GetRotating())
			{
				playerRight = true;
				if (playerLeft)
				{
					camera.ChangeCameraTarget(-1);
					playerLeft = false;
				}
				if (camera.CameraTargetReached())
				{
					camera.SetRotating(false);
					playerRight = false;
				}
				else
				{
					camera.RotateCam(-1);
					world_->SetGravity(b2Vec2(-gravityAmount * camera.GetCameraUp().x(), -gravityAmount * camera.GetCameraUp().y()));
				}
			}
			else if (b2Abs(player_body_->GetLinearVelocity().x) < 0.1f && b2Abs(player_body_->GetLinearVelocity().y) < 0.1f && playerRight)
			{
				//rotate
				camera.SetRotating(true);
				camera.ChangeCameraTarget(-1);
				player_body_->SetLinearVelocity(b2Vec2_zero);
				
			}

			else
			{
				//player_body_->SetLinearVelocity(b2Vec2((3 * camera.GetCameraRight().x) + world_->GetGravity().x, (3 * camera.GetCameraRight().y) + world_->GetGravity().y));
				//player_body_->ApplyForceToCenter(b2Vec2((camera.GetCameraRight().x), (camera.GetCameraRight().y)), false);

				if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
				{
					player_body_->SetLinearVelocity(b2Vec2(3 * camera.GetCameraRight().x, player_body_->GetLinearVelocity().y));
				}
				else
				{
					player_body_->SetLinearVelocity(b2Vec2(player_body_->GetLinearVelocity().x, 3 * camera.GetCameraRight().y));
				}
				
				playerRight = true;
			}

			playerLeft = false;
			
		}
		else if (keyboard->IsKeyDown(gef::Keyboard::KC_LEFT))
		{
			if (camera.GetRotating())
			{
				playerLeft = true;
				if (playerRight)
				{
					camera.ChangeCameraTarget(1);
					playerRight = false;
				}
				if (camera.CameraTargetReached())
				{
					camera.SetRotating(false);
					playerLeft = false;
				}
				else
				{
					camera.RotateCam(1);
					world_->SetGravity(b2Vec2(-gravityAmount * camera.GetCameraUp().x(), -gravityAmount * camera.GetCameraUp().y()));
				}
			}
			else if (b2Abs(player_body_->GetLinearVelocity().x) < 0.1f && b2Abs(player_body_->GetLinearVelocity().y) < 0.1f && playerLeft)
			{
				//rotate
				camera.SetRotating(true);
				camera.ChangeCameraTarget(1);
				player_body_->SetLinearVelocity(b2Vec2_zero);
			}

			else
			{
				
				//player_body_->ApplyForceToCenter(b2Vec2((camera.GetCameraRight().x), (camera.GetCameraRight().y)), false);
				//player_body_->SetLinearVelocity(b2Vec2((-3 * camera.GetCameraRight().x) + world_->GetGravity().x, (-3 * camera.GetCameraRight().y) + world_->GetGravity().y));
				if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
				{
					player_body_->SetLinearVelocity(b2Vec2(-3 * camera.GetCameraRight().x, player_body_->GetLinearVelocity().y));
				}
				else
				{
					player_body_->SetLinearVelocity(b2Vec2(player_body_->GetLinearVelocity().x, -3 * camera.GetCameraRight().y));
				}
				playerLeft = true;
			}

			playerRight = false;

		}
		else
		{
			if (camera.GetCameraTarget() == 0 || camera.GetCameraTarget() == 2)
			{
				player_body_->SetLinearVelocity(b2Vec2(0.0f, player_body_->GetLinearVelocity().y));
			}
			else
			{
				player_body_->SetLinearVelocity(b2Vec2(player_body_->GetLinearVelocity().x, 0.0f));
			}
			//player_body_->SetLinearVelocity(b2Vec2(player_body_->GetLinearVelocity().x, player_body_->GetLinearVelocity().y));
			//player_body_->SetLinearVelocity(b2Vec2(world_->GetGravity().x, world_->GetGravity().y));
			if (!camera.GetRotating())
			{
				playerLeft = false;
				playerRight = false;
			}
		}

		if (keyboard->IsKeyDown(gef::Keyboard::KC_SPACE))
		{
			enemy->Shoot(b2Vec2(0, 100));
		}
	}
}



void SceneApp::Render()
{
	// setup camera

	// projection
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_.width() / (float)platform_.height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);
	
	// view
	gef::Vector4 camera_eye(player_body_->GetPosition().x, player_body_->GetPosition().y, 30.0f);
	gef::Vector4 camera_lookat(player_body_->GetPosition().x, player_body_->GetPosition().y, 0.0f);
	
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

	for (int i = 0; i < levelWidth; i++)
	{
		for (int j = 0; j < levelHeight; j++)
		{
			renderer_3d_->DrawMesh(levelBuilder->GetBlock(i, j));
		}
	}

	// draw player
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	renderer_3d_->DrawMesh(player_);
	renderer_3d_->set_override_material(NULL);

	//rendenemy
	renderer_3d_->set_override_material(&primitive_builder_->blue_material());
	renderer_3d_->DrawMesh(*enemy);
	renderer_3d_->set_override_material(NULL);

	//
	renderer_3d_->set_override_material(&primitive_builder_->red_material());
	if (!enemy->GetCanShoot())
	{
		renderer_3d_->DrawMesh(*enemy->GetBulletMesh());
	}
	renderer_3d_->set_override_material(NULL);

	renderer_3d_->End();

	// start drawing sprites, but don't clear the frame buffer
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

void SceneApp::InitPlayer()
{
	// setup the mesh for the player
	player_.set_mesh(primitive_builder_->GetDefaultCubeMesh());

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.fixedRotation = true;
	//player_body_def.position = levelBuilder->GetStartPosition();
	player_body_def.position = b2Vec2(5, 5);

	player_body_ = world_->CreateBody(&player_body_def);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(0.5f, 0.5f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	player_body_->CreateFixture(&player_fixture_def);

	//set player gamebject type
	player_.SetType(PLAYER);

	// update visuals from simulation data
	player_.UpdateFromSimulation(player_body_);

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

void SceneApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void SceneApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
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
