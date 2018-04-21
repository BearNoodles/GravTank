#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Camera.h"
#include "GameManager.h"
#include "Enemy.h"
#include "Player.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void UpdatePlaying();
	void Render();
private:
	void InitPlayer();
	void InitGround();
	void InitBuildings();
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void ProcessControllerInput();
	void ProcessKeyboardInput();

	GameObject* toDel;
    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	gef::InputManager* input_manager_;

	PrimitiveBuilder* primitive_builder_;

	// create the physics world
	b2World* world_;

	// player variables
	Player* player;

	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;

	//levelBuild 
	GameManager* gameManager;
	int levelWidth = 15;
	int levelHeight = 15;

	//building variables
	gef::Mesh* buildingMesh;
	GameObject building;
	b2Body* buildingBody;
	GameObject building2;
	b2Body* buildingBody2;

	Camera camera;

	//Gravity
	float gravityAmount;

	float fps_;
};

#endif // _SCENE_APP_H
