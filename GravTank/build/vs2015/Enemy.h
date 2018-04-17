#pragma once
#include "game_object.h"
#include "primitive_builder.h"
#include "Bullet.h"


class Enemy : public GameObject
{
public:
	Enemy(int type, b2World* world_, PrimitiveBuilder* builder);
	~Enemy();
	void Update(b2Vec2 gravity);
	void ReduceBulletCount();
	void Shoot(b2Vec2 force);
	bool GetCanShoot();
	gef::MeshInstance* GetBulletMesh();

private:
	void Init();
	Bullet* bullet;
	bool canShoot;
	int changeTimer;
	int changeAt;
	int direction;
	float speed;
	int enemyType;

	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;
};

