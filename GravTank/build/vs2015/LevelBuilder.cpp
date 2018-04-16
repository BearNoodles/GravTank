#include "LevelBuilder.h"
#include <graphics/mesh.h>



LevelBuilder::LevelBuilder(b2World* world, PrimitiveBuilder* builder) :
	world_(world),
	primitive_builder_(builder)
{

}

void LevelBuilder::LoadLevel()
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			switch (level1[i][j])
			{
			case 0:
				break;

			case 1:
				// building dimensions
				gef::Vector4 building_half_dimensions(2.0f, 2.0f, 0.5f);
				
				// setup the mesh for the buildings
				levelBlocks[i][j].blockMesh = primitive_builder_->CreateBoxMesh(building_half_dimensions);
				levelBlocks[i][j].blockObject.set_mesh(levelBlocks[i][j].blockMesh);
				
				// create physics bodies
				b2BodyDef body_def;
				body_def.type = b2_staticBody;
				body_def.position = b2Vec2(4 * i, 4 * j);
				
				levelBlocks[i][j].blockBody = world_->CreateBody(&body_def);
				
				// create the shape
				b2PolygonShape shape;
				shape.SetAsBox(building_half_dimensions.x(), building_half_dimensions.y());
				
				// create the fixture
				b2FixtureDef fixture_def;
				fixture_def.shape = &shape;
				
				// create the fixtures on the rigid bodies
				levelBlocks[i][j].blockBody->CreateFixture(&fixture_def);
				
				// update visuals from simulation data
				levelBlocks[i][j].blockObject.UpdateFromSimulation(levelBlocks[i][j].blockBody);
				break;
			}

		}
	}
}

GameObject LevelBuilder::GetBlock(int i, int j)
{
	return levelBlocks[i][j].blockObject;
}

b2Vec2 LevelBuilder::GetStartPosition()
{
	return StartPosition;
}

void LevelBuilder::RenderLevel()
{

}

void LevelBuilder::SetLevels()
{
	int temp[10][10]{ { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 },
					  { 1,1,1,1,1,1,1,1,1,1 } };
}


LevelBuilder::~LevelBuilder()
{
}
