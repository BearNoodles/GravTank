#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Camera.h"
#include "Menu.h"
#include "GameManager.h"
#include "Enemy.h"
#include "Player.h"
#include "graphics\sprite.h"
#include "graphics\texture.h"
#include "graphics\image_data.h"
#include "assets\png_loader.h"
#include <audio/audio_manager.h>


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
	class Material;
	class Texture;
	class PngLoader;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);
	void Init();
	void CleanUp();

	bool Update(float frame_time);
	
	void UpdatePlaying(float frame_time); //Different update functions depending on gamestate
	void UpdateMenu();
	void UpdatePause();

	void Render();
private:
	void InitBackground();
	void InitFont();
	void InitHealthSprite();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void ProcessControllerInput();
	void ProcessKeyboardInput();

	//Reloads either current level or loads next level depeding on the bool argument
	void ResetLevel(bool nextLevel);
	//Special case reload for when player dies
	void PlayerDeath();

    
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	gef::InputManager* input_manager_;
	
	void RightPressed(); //Function for when player presses a direction
	void LeftPressed();
	void StopPlayer();

	PrimitiveBuilder* primitive_builder_;

	// create the physics world
	b2World* world_;

	// player variables
	Player* player;
	float playerSpeed;
	gef::Sprite healths[6];
	int playerHealth;

	int enemiesLeft;
	int levelNo;

	bool L2Pressed;

	float rightStickX;
	float rightStickY;

	// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	b2Body* ground_body_;

	//Gamemanager creates levels and handles gamestate 
	GameManager* gameManager;
	int levelWidth = 15;
	int levelHeight = 15;

	//Background meshs
	gef::Mesh* backMesh;
	GameObject back;
	gef::Mesh* backMesh2;
	GameObject back2;

	//Controls the moving camera
	Camera camera;

	//Front menu
	Menu* menu;
	int screenWidth;
	int screenHeight;

	//Gravity
	float gravityAmount;

	float fps_;

	float fpsScale; // for scaling speeds up when fps drops

	//Image texture material and sprites
	gef::PNGLoader pngLoader;
	
	gef::ImageData healthImage;
	gef::Texture* healthTexture;

	gef::ImageData playerImage;
	gef::Texture* playerTexture;
	gef::Material* playerMaterial;

	gef::ImageData enemyImage;
	gef::Texture* enemyTexture;
	gef::Material* enemyMaterial;

	gef::ImageData explodeImage;
	gef::Texture* explodeTexture;
	gef::Material* explodeMaterial;

	gef::ImageData tileImage;
	gef::Texture* tileTexture;
	gef::Material* tileMaterial;

	gef::ImageData backImage;
	gef::Texture* backTexture;
	gef::Material* backMaterial;

	gef::ImageData blackImage;
	gef::Texture* blackTexture;
	gef::Material* blackMaterial;
	gef::Sprite blackSprite;

	gef::ImageData menuImage;
	gef::Texture* menuTexture;
	gef::Material* menuMaterial;
	gef::Sprite menuSprite;

	gef::ImageData triImage;
	gef::Texture* triTexture;
	gef::Material* triMaterial;
	gef::Sprite triSprite;

	gef::ImageData startImage;
	gef::Texture* startTexture;
	gef::Material* startMaterial;
	gef::Sprite startSprite;

	gef::ImageData pauseImage;
	gef::Texture* pauseTexture;
	gef::Sprite pauseSprite;

	void InitTextures();

	void DrawMenu();

	gef::AudioManager* audio_manager_;

	//Sound effect ids
	int sfx_id_shoot;
	int sfx_id_move;
	int sfx_id_explode;
};

#endif // _SCENE_APP_H
