#pragma once
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"

namespace gef
{
	class Platform;
}

struct Block
{
	gef::Mesh* blockMesh;
	GameObject blockObject;
	b2Body* blockBody;
};

class LevelBuilder
{
public:
	LevelBuilder(gef::Platform &platform_, b2World &world_);
	~LevelBuilder();

	void LoadLevel();
	void RenderLevel();
	GameObject GetBlock(int i, int j);
	b2Vec2 GetStartPosition();

private:

	const int width = 15;
	const int height = 15;

	gef::Platform& platform_;
	b2World& world_;
	//
	PrimitiveBuilder* primitive_builder_;
	b2Vec2 StartPosition;

	Block levelBlocks[15][15];
	int level1[15][15]{ { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 },
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }, 
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }, 
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }, 
						{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }, 
						{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } };
	int level2[10][10]{ { 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 } };
	int level3[10][10]{ { 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 } };

	void SetLevels();
};

