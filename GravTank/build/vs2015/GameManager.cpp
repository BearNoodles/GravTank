#include "GameManager.h"
#include <graphics/mesh.h>



GameManager::GameManager(b2World* world, PrimitiveBuilder* builder, gef::AudioManager* audioManager, int shootID, int moveID) :
	m_world(world),
	m_builder(builder)
{
	//start in menu state
	m_state = MENU;
	//level 1 first
	currentLevel = 1;

	enemyCount = 0;

	//difficulty out of 3, higher difficulty means more enemies
	m_difficulty = 1;

	m_audioManager = audioManager;
	sfx_id_shoot = shootID;
	sfx_id_move = moveID;
	voice_id_shoot = -1;
	voice_id_move = -1;
}

void GameManager::NextLevel()
{
	//keep difficulty between 1 and 3
	currentLevel++;
	if (currentLevel > 3)
	{
		currentLevel = 1;
	}
}

int GameManager::GetLevelNo()
{
	return currentLevel;
}

void GameManager::LoadLevel()
{
	//temp array to hold current level values
	int level[15][15];

	//set up temp array with the current level
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

	//dimensions of a block
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
				//0 gives an empty space in the level
				case 0:
					SetBlockNull(i, j);
					break;

				// 1 loads a level block in that position, with a body so objects can make contact with it
				case 1:
					// setup the mesh for the block
					levelBlocks[i][j].blockObject = new GameObject();
					levelBlocks[i][j].blockMesh = m_builder->CreateBoxMesh(building_half_dimensions);
					levelBlocks[i][j].blockObject->set_mesh(levelBlocks[i][j].blockMesh);

					// create physics body
					body_def.type = b2_staticBody;
					body_def.position = b2Vec2(4 * j, 4 * -i);

					levelBlocks[i][j].blockBody = m_world->CreateBody(&body_def);
					levelBlocks[i][j].blockBody->SetUserData(levelBlocks[i][j].blockObject);

					// create the shape
					shape.SetAsBox(building_half_dimensions.x(), building_half_dimensions.y());

					// create the fixture
					fixture_def.shape = &shape;

					// create the fixture on the rigid body
					levelBlocks[i][j].blockBody->CreateFixture(&fixture_def);

					// update visuals from simulation data
					levelBlocks[i][j].blockObject->UpdateFromSimulation(levelBlocks[i][j].blockBody);

					//set gameobject type to tile
					levelBlocks[i][j].blockObject->SetType(TILE);

					break;
				
				//2 set the player start position to the current position (no block)
				case 2:
					SetBlockNull(i, j);
					
					StartPosition = b2Vec2((4 * j), (4 * -i));
					break;

				// 3 loads a type 3 enemy (normal gravity) in the current array position
				case 3:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(3, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
					enemyCount++;
					break;

				// 4 loads a type 4 enemy (left)
				case 4:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(4, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
					enemyCount++;
					break;

				// type 5 enemy (up)
				case 5:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(5, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
					enemyCount++;
					break;

				// type 6 enemy (right)
				case 6:
					SetBlockNull(i, j);
					enemies[enemyCount] = new Enemy(6, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
					enemyCount++;
					break;

				// 7 loads a type 3 enemy on normal difficulty or higher, an empty space on lower difficulties
				case 7:
					if (m_difficulty > 1)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(3, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;
				// normal difficulty type 4 enemy
				case 8:
					if (m_difficulty > 1)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(4, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;
				// normal difficulty type 5 enemy
				case 9:
					if (m_difficulty > 1)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(5, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;

				//normal difficulty type 6 enemy
				case 10:
					if (m_difficulty > 1)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(6, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else 
						SetBlockNull(i, j);
					break;

				//hard difficulty type 3
				case 11:
					if (m_difficulty > 2)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(3, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;

				//hard difficulty type 4
				case 12:
					if (m_difficulty > 2)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(4, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;

				//hard difficulty type 5
				case 13:
					if (m_difficulty > 2)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(5, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;

				//hard difficulty type 6
				case 14:
					if (m_difficulty > 2)
					{
						SetBlockNull(i, j);
						enemies[enemyCount] = new Enemy(6, m_world, m_builder, b2Vec2((4 * j), (4 * -i)), m_audioManager, sfx_id_shoot, sfx_id_move);
						enemyCount++;
					}
					else
						SetBlockNull(i, j);
					break;
			}
		}
	}

	enemiesAlive = enemyCount;
}

void GameManager::SetBlockNull(int i, int j)
{
	//set a block to NULL
	levelBlocks[i][j].blockObject = NULL;
	levelBlocks[i][j].blockBody = NULL;
	levelBlocks[i][j].blockMesh = NULL;
}

//set all blocks and enemies to null to prepare for loading next level
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

void GameManager::ResetAll()
{
	//reset level and then return to menu
	Reset();
	currentLevel = 1;
	m_state = MENU;
}

void GameManager::Update(b2Vec2 gravity, b2Vec2 playerPos, float fpsScale)
{
	//loops hrough all enemies and updates them
	for (int i = 0; i < enemyCount; i++)
	{
		enemies[i]->Update(gravity, playerPos, fpsScale);
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

void GameManager::ChangeDifficulty()
{
	m_difficulty++;
	if (m_difficulty > 3)
	{
		m_difficulty = 1;
	}
}

int GameManager::GetDifficulty()
{
	return m_difficulty;
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
