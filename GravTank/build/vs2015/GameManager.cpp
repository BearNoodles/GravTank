#include "GameManager.h"
#include <graphics/mesh.h>



GameManager::GameManager(b2World* world, PrimitiveBuilder* builder) :
	m_world(world),
	m_builder(builder)
{
	m_state = PLAYING;
	currentLevel = 1;
	enemyCount = 0;
}

void GameManager::NextLevel()
{
	currentLevel++;
	if (currentLevel > 3)
	{
		currentLevel = 1;
	}
}

void GameManager::LoadLevel()
{
	int level[15][15];

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			switch (currentLevel)
			{
				case 1:
					level[i][j] = level1[i][j];
					break;
				case 2:
					level[i][j] = level2[i][j];
					break;
				case 3:
					level[i][j] = level3[i][j];
					break;
			}
		}
	}

	gef::Vector4 building_half_dimensions(2.0f, 2.0f, 0.5f);

	b2FixtureDef fixture_def;

	b2BodyDef body_def;

	b2PolygonShape shape;
	
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			switch (level[i][j])
			{
				case 0:
					SetBlockNull(i, j);
					break;

				case 1:
					// setup the mesh for the buildings
					levelBlocks[i][j].blockObject = new GameObject();
					levelBlocks[i][j].blockMesh = m_builder->CreateBoxMesh(building_half_dimensions);
					levelBlocks[i][j].blockObject->set_mesh(levelBlocks[i][j].blockMesh);

					// create physics bodies
					body_def.type = b2_staticBody;
					body_def.position = b2Vec2(4 * j, 4 * -i);

					levelBlocks[i][j].blockBody = m_world->CreateBody(&body_def);
					levelBlocks[i][j].blockBody->SetUserData(levelBlocks[i][j].blockObject);

					// create the shape
					shape;
					shape.SetAsBox(building_half_dimensions.x(), building_half_dimensions.y());

					// create the fixture
					fixture_def;
					fixture_def.shape = &shape;

					// create the fixtures on the rigid bodies
					levelBlocks[i][j].blockBody->CreateFixture(&fixture_def);

					// update visuals from simulation data
					levelBlocks[i][j].blockObject->UpdateFromSimulation(levelBlocks[i][j].blockBody);

					levelBlocks[i][j].blockObject->SetType(TILE);

					break;

				case 2:
					SetBlockNull(i, j);
					
					StartPosition = b2Vec2((4 * j), (4 * -i));
					break;

				case 3:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(3, m_world, m_builder, b2Vec2((4 * j), (4 * -i)));
					enemyCount++;
					break;

				case 4:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(4, m_world, m_builder, b2Vec2((4 * j), (4 * -i)));
					enemyCount++;
					break;

				case 5:
					SetBlockNull(i, j);
					//levelBlocks[i][j].blockObject = NULL;
					levelBlocks[i][j].blockBody = NULL;
					levelBlocks[i][j].blockMesh = NULL;
					enemies[enemyCount] = new Enemy(5, m_world, m_builder, b2Vec2((4 * j), (4 * -i)));
					enemyCount++;
					break;

				case 6:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(6, m_world, m_builder, b2Vec2((4 * j), (4 * -i)));
					enemyCount++;
					break;
			}
		}
	}

	enemiesAlive = enemyCount;
}

void GameManager::SetBlockNull(int i, int j)
{
	levelBlocks[i][j].blockObject = NULL;
	levelBlocks[i][j].blockBody = NULL;
	levelBlocks[i][j].blockMesh = NULL;
}

void GameManager::Reset()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			delete levelBlocks[i][j].blockMesh;
			levelBlocks[i][j].blockMesh = NULL;
			delete levelBlocks[i][j].blockObject;
			levelBlocks[i][j].blockObject = NULL;

			if (levelBlocks[i][j].blockBody != NULL)
			{
				m_world->DestroyBody(levelBlocks[i][j].blockBody);
				levelBlocks[i][j].blockBody = NULL;
			}
		}
	}
	for (int i = 0; i < enemyCount; i++)
	{
		delete enemies[i];
		enemies[i] = NULL;
	}
	enemyCount = 0;;

}

void GameManager::Update(b2Vec2 gravity)
{
	for (int i = 0; i < enemyCount; i++)
	{
		enemies[i]->Update(gravity);
	}
}

void GameManager::ReduceEnemyCount()
{
	enemiesAlive--;
}

GameObject* GameManager::GetBlock(int i, int j)
{
	return levelBlocks[i][j].blockObject;
}

Enemy* GameManager::GetEnemy(int i)
{
	return enemies[i];
}

int GameManager::GetEnemyCount()
{
	return enemyCount;
}

int GameManager::GetEnemiesAlive()
{
	return enemiesAlive;
}

void GameManager::SetEnemiesAlive(int value)
{
	enemiesAlive = value;
}

gef::MeshInstance* GameManager::GetEnemyMesh(int i)
{
	return enemies[i];
}

b2Vec2 GameManager::GetStartPosition()
{
	return StartPosition;
}

GameState GameManager::GetState()
{
	return m_state;
}

void GameManager::SetState(GameState state)
{
	m_state = state;
}

void GameManager::RenderLevel()
{

}

void GameManager::SetLevels()
{
}


GameManager::~GameManager()
{
}
