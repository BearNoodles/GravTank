#pragma once
#include "game_object.h"

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();
	void Update();
	void SetVelocity(b2Vec2 vel);
};

