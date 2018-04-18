#pragma once
#include "game_object.h"
#include "Bullet.h"

class Player : public GameObject
{
public:
	Player(b2World* world_, PrimitiveBuilder* builder);
	~Player();

	void Update();
	void Shoot(b2Vec2 force);
	bool GetCanShoot();
	gef::MeshInstance* GetBulletMesh();
	b2Vec2 GetPosition();
	b2Vec2 GetVelocity();
	void SetVelocity(b2Vec2 vel);
	bool GetPlayerRight();
	bool GetPlayerLeft();
	void SetPlayerRight(bool value);
	void SetPlayerLeft(bool value);

private:
	void Init();
	void CreateBullet();
	Bullet* bullet;
	bool canShoot;
	int direction;
	float speed;

	bool playerRight;
	bool playerLeft;

	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;
};

