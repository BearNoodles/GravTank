#pragma once
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Enemy.h"

//blocks make up the level layout
struct Block
{
	gef::Mesh* blockMesh;
	GameObject* blockObject;
	b2Body* blockBody;
};

//current state of game
enum GameState
{
	MENU, PLAYING, PAUSE
};

class GameManager
{
public:
	GameManager(b2World* world_, PrimitiveBuilder* builder, gef::AudioManager* audioManager, int shootID, int moveID);
	~GameManager();

	void LoadLevel();
	void Reset();
	void ResetAll();
	void RenderLevel();
	GameObject* GetBlock(int i, int j);
	b2Vec2 GetStartPosition();
	void NextLevel();
	GameState GetState();
	void SetState(GameState state);
	Enemy* GetEnemy(int i);
	int GetEnemyCount();
	int GetEnemiesAlive();
	void SetEnemiesAlive(int value);
	void ReduceEnemyCount();
	gef::MeshInstance* GetEnemyMesh(int i);

	void Update(b2Vec2 gravity, b2Vec2 playerPos, float fpsScale);
	void ChangeDifficulty();
	int GetDifficulty();
	int GetLevelNo();
private:
	const int width = 15;
	const int height = 15;

	int m_difficulty;

	GameState m_state;
	int currentLevel;

	b2World* m_world;
	//
	PrimitiveBuilder* m_builder;
	b2Vec2 StartPosition;
	int enemyCount;
	int enemiesAlive;
	Enemy* enemies[20];


	gef::AudioManager* m_audioManager;
	int sfx_id_shoot;
	int sfx_id_move;
	int voice_id_shoot;
	int voice_id_move;

	void SetBlockNull(int i, int j);

	//these arrays represent the level layout, with different numbers loading different game objects in that position
	Block levelBlocks[15][15];
	int level1[15][15]{ {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1,13, 0, 0, 0, 0, 0,13, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 2, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 0,10, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,12, 0, 0, 1 },
						{  1, 5, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1, 0, 0, 1 },
						{  1, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 7, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

	int level2[15][15]{ {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 2, 0, 0, 0,13, 0, 5, 0, 0, 0, 0, 0, 9, 1 },
						{  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  0, 0, 1, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 6, 1 },
						{  0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0,12, 1 },
						{  0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 1, 7, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,10, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 1 },
						{  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 } };

	int level3[15][15]{ {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 4, 0, 0, 0, 5, 0, 0, 0, 3, 0, 0, 0,10, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
						{  1, 0, 0, 0,12, 0, 0, 0, 8, 0, 5, 0, 0, 6, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 4, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 0, 0, 0, 0, 0, 5, 9, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
						{  1,12, 0, 0, 1,13, 0, 0, 3, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0,10, 1 },
						{  1, 2, 0, 0, 1, 7, 0, 0, 0, 7, 0, 3, 0, 0, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

	void SetLevels();
};

