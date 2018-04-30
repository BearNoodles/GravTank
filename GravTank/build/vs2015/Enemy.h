#pragma once
#include "game_object.h"
#include "primitive_builder.h"
#include "Bullet.h"
#include "graphics\material.h"
#include <audio/audio_manager.h>


class Enemy : public GameObject
{
public:
	Enemy(int type, b2World* world_, PrimitiveBuilder* builder, b2Vec2 startPosition, gef::AudioManager* audioManager, int shootID, int moveID);
	~Enemy();
	void Update(b2Vec2 gravity, b2Vec2 aim, float fpsScale);
	void Shoot(b2Vec2 force);
	bool GetCanShoot();
	gef::MeshInstance* GetBulletMesh();
	b2Vec2 GetPosition();
	void Die();
	bool GetDead();

private:
	void Init();
	void CreateBullet();
	Bullet* bullet;
	bool canShoot;

	float shotForce;

	float bulletForce;
	float changeTimer;
	int changeAt;
	int direction;
	float speed;
	int enemyType;

	bool isDead;

	b2Vec2 startPosition;

	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;

	gef::AudioManager* m_audioManager;
	int sfx_id_shoot;
	int sfx_id_move;
	int voice_id_shoot;
	int voice_id_move;
};

