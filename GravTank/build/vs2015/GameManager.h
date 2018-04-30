#pragma once
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "Enemy.h"

struct Block
{
	gef::Mesh* blockMesh;
	GameObject* blockObject;
	b2Body* blockBody;
};

enum GameState
{
	MENU, PLAYING, LOADING
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
	Enemy* enemies[15];


	gef::AudioManager* m_audioManager;
	int sfx_id_shoot;
	int sfx_id_move;
	int voice_id_shoot;
	int voice_id_move;

	void SetBlockNull(int i, int j);

	Block levelBlocks[15][15];
	int level1[15][15]{ {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 0, 0, 0, 0, 5, 0, 0, 0, 3, 0, 0, 0, 0, 1 },
						{  1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 
						{  1, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, 
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1 }, 
						{  1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1 }, 
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };
	int level2[15][15]{ {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };
	int level3[15][15]{ {  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
						{  1, 0, 2, 7, 0, 0, 7, 0, 3, 0, 0, 0, 0, 0, 1 },
						{  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };

	void SetLevels();
};

