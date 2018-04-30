#pragma once
#include "game_object.h"
#include "Bullet.h"
#include "Explosion.h"
#include <box2d/Box2D.h>
#include <audio/audio_manager.h>

class Player : public GameObject
{
public:
	Player(b2World* world_, PrimitiveBuilder* builder, b2Vec2 startPos, gef::AudioManager* audioManager, int shootID, int moveID, int explodeID);
	~Player();

	void Update(float x, float y, b2Vec2 up);
	void Shoot(float shotScale);
	void ExplodeBullet();
	bool GetCanShoot();
	void SetCanShoot(bool value);
	gef::MeshInstance* GetBulletMesh();
	gef::MeshInstance* GetExplosionMesh();
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

	void SetMoveSound(bool playing);

	bool IsExploding();
	
	GameObject* GetTurret();
	void SetTurretAngle(float radians);

private:
	void InitBody();
	int health;
	int maxHealth;
	void CreateBullet();
	void CreateExplosion();
	Bullet* bullet;
	Explosion* explode;
	bool canShoot;
	float speed;
	float bulletForce;

	

	bool playerRight;
	bool playerLeft;

	void CreateTurret();
	GameObject* turret;
	b2Body* m_turretBody;

	float m_frameTime;
	
	void PositionTurret(float x, float y);

	float stickX;
	float stickY;
	float turretRot;

	b2Body* m_body;
	b2World* m_world;
	PrimitiveBuilder* m_builder;

	gef::Material* m_material;

	b2Vec2 startPosition;

	gef::AudioManager* m_audioManager;
	int sfx_id_shoot;
	int sfx_id_move;
	int sfx_id_explode;
	int voice_id_shoot;
	int voice_id_move;
	int voice_id_explode;
};

