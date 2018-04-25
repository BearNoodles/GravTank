#pragma once
#include "game_object.h"
#include "primitive_builder.h"
#include "Bullet.h"
#include "graphics\material.h"


class Enemy : public GameObject
{
public:
	Enemy(int type, b2World* world_, PrimitiveBuilder* builder, b2Vec2 startPosition);
	~Enemy();
	void Update(b2Vec2 gravity);
	void Shoot(b2Vec2 force);
	bool GetCanShoot();
	gef::MeshInstance* GetBulletMesh();
	b2Vec2 GetPosition();
	void BulletCollision();
	void Die();
	bool GetDead();

private:
	void Init();
	void CreateBullet();
	Bullet* bullet;
	bool canShoot;

	float bulletForce;
	int changeTimer;
	int changeAt;
	int direction;
	float speed;
	int enemyType;

	bool isDead;

	b2Vec2 startPosition;

	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;
};

