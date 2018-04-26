#pragma once
#include "game_object.h"
#include "Bullet.h"
#include <box2d/Box2D.h>

class Player : public GameObject
{
public:
	Player(b2World* world_, PrimitiveBuilder* builder, b2Vec2 startPos, gef::Material* mat);
	~Player();

	void Update(float x, float y);
	void Shoot(b2Vec2 force);
	bool GetCanShoot();
	void SetCanShoot(bool value);
	gef::MeshInstance* GetBulletMesh();
	b2Vec2 GetPosition();
	void SetPosition(b2Vec2);
	b2Vec2 GetVelocity();
	void SetVelocity(b2Vec2 vel);
	bool GetPlayerRight();
	bool GetPlayerLeft();
	void SetPlayerRight(bool value);
	void SetPlayerLeft(bool value);
	int GetHealth();
	int GetMaxHealth();
	void SetHealth(int value);
	void ReduceHealth();
	void ResetPlayer(b2Vec2 startPos);
	
	
	GameObject* GetTurret();
	void SetTurretAngle(float radians);

private:
	void Init();
	int health;
	int maxHealth;
	void CreateBullet();
	Bullet* bullet;
	bool canShoot;
	float speed;
	float bulletForce;

	

	bool playerRight;
	bool playerLeft;

	void CreateTurret();
	GameObject* turret;
	b2Body* m_turretBody;

	
	void PositionTurret(float x, float y);

	float stickX;
	float stickY;
	float turretRot;

	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;

	gef::Material* m_material;

	b2Vec2 startPosition;
};

