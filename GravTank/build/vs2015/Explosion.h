#pragma once
#include "game_object.h"
#include "primitive_builder.h"

class Explosion : public GameObject
{
public:
	Explosion(b2Vec2 pos, b2World* world, PrimitiveBuilder* builder);
	~Explosion();
	void CleanUp();

	void Update();
	b2Body* GetBody();

	void Activate(b2Vec2 pos);
	bool IsActive();

	void Reset();

private:
	void Init();

	int activeCount;
	int timer;
	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;
};
