#pragma once
#include "game_object.h"
#include "primitive_builder.h"


class Bullet : public GameObject
{
public:
	Bullet(b2Vec2 force, b2Vec2 pos, b2World* world, PrimitiveBuilder* builder);
	~Bullet();

	void Update();

private:
	void Init();
	b2Vec2 force;
	
	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;
};

