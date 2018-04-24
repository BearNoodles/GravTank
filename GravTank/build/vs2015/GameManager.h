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
	GameObject blockObject;
	b2Body* blockBody;
};

enum GameState
{
	MENU, PLAYING, LOADING
};

class GameManager
{
public:
	GameManager(b2World* world_, PrimitiveBuilder* builder);
	~GameManager();

	void LoadLevel();
	void Reset();
	void RenderLevel();
	GameObject GetBlock(int i, int j);
	b2Vec2 GetStartPosition();
	void NextLevel();
	GameState GetState();
	void SetState(GameState state);
	Enemy* GetEnemy(int i);
	int GetEnemyCount();
	void ReduceEnemyCount();
	gef::MeshInstance* GetEnemyMesh(int i);

	void Update(b2Vec2 gravity);

private:

	const int width = 15;
	const int height = 15;

	GameState m_state;
	int currentLevel;

	b2World* m_world;
	//
	PrimitiveBuilder* m_builder;
	b2Vec2 StartPosition;
	int enemyCount;
	Enemy* enemies[15];

	Block levelBlocks[15][15];
	int level1[15][15]{ { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
						{ 1,2,0,0,0,4,0,0,0,4,0,0,0,0,1 },
						{ 1,1,3,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,3,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,3,0,0,0,0,0,0,1,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,6,1,0,0,0,0,1 },
						{ 1,0,0,0,0,0,1,0,1,1,0,0,0,0,1 },
						{ 1,0,0,0,0,0,5,1,0,1,0,0,0,0,1 },
						{ 1,1,0,0,0,0,1,0,1,1,0,0,0,5,1 },
						{ 1,0,0,0,0,0,0,0,0,1,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,1,0,0,0,0,1 }, 
						{ 1,1,0,6,0,0,0,0,0,1,0,0,0,0,1 }, 
						{ 1,0,0,0,0,0,0,0,0,0,0,6,0,0,1 }, 
						{ 1,0,1,0,1,0,1,0,0,1,0,0,1,0,1 }, 
						{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } };
	int level2[15][15]{ { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
						{ 1,2,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } };
	int level3[15][15]{ { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } };

	void SetLevels();
};

