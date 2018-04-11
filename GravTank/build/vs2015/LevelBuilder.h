#pragma once
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include "game_object.h"

#define WIDTH = 10
#define HEIGHT = 10

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

	void Init(gef::Platform &platform_, b2World &world_);
	void LoadLevel();
	void RenderLevel();
	GameObject GetBlock(int i, int j);

private:

	const int width = 10;
	const int height = 10;

	gef::Platform& platform_;
	b2World& world_;
	//
	PrimitiveBuilder* primitive_builder_;

	Block levelBlocks[10][10];
	int level1[10][10]{ { 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 },
						{ 1,1,1,1,1,1,1,1,1,1 } };
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

