#include "Enemy.h"



Enemy::Enemy(int type, b2World* world, PrimitiveBuilder* builder, b2Vec2 start, gef::AudioManager* audioManager, int shootID, int moveID) :
	m_world(world),
	m_builder(builder),
	bullet(NULL)
{
	//set gameobject type to enemy
	enemyType = type;
	//set start posision to given position
	startPosition = start;

	Init();
	CreateBullet();

	//start with the ability to shoot
	canShoot = true;
	
	isDead = false;

	//set up enemy sound effects from given ids
	m_audioManager = audioManager;
	sfx_id_shoot = shootID;
	sfx_id_move = moveID;
	voice_id_shoot = -1;
	voice_id_move = -1;

	//force which bullets are fired
	shotForce = 200;
	//distance at which enemy will shoot at player
	maxShootLength = 20.5f;
}

void Enemy::Init()
{
	//keep enemy tyoe from 3 to 6 for the level builder
	if (enemyType > 6)
		enemyType = 6;
	else if (enemyType < 3)
		enemyType = 3;

	//timer values for changing direction and shooting
	changeTimer = 0;
	changeAt = 160;

	//enemy speed
	speed = 3.0f;

	direction = 1;

	// setup the mesh for the Enemy
	set_mesh(m_builder->CreateSphereMesh(1.0f, 10, 10));

	// create a physics body for the enemy
	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;
	enemy_body_def.fixedRotation = false;
	enemy_body_def.position = startPosition;

	m_body = m_world->CreateBody(&enemy_body_def);
	m_body->SetUserData(this);

	// create the shape for the enemy
	b2CircleShape enemy_shape;
	enemy_shape.m_p.SetZero();
	enemy_shape.m_radius = 1;

	// create the fixture
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	m_body->CreateFixture(&enemy_fixture_def);

	//set enemy gamebject type
	SetType(ENEMY);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);
}

void Enemy::Die()
{
	//dead enemies are set as inactive until level is complete
	m_body->SetActive(false);
	isDead = true;
}

bool Enemy::GetDead()
{
	return isDead;
}

void Enemy::CreateBullet()
{
	bullet = new Bullet(m_body->GetPosition(), m_world, m_builder);
	bullet->SetBulletType(ENEMYBULLET);
	canShoot = true;
}

void Enemy::Update(b2Vec2 gravity, b2Vec2 playerPos, float fpsScale)
{
	//set gravity based on the current fps
	gravity *= fpsScale;

	//if the enemy is dead dont update further
	if (!m_body->IsActive())
	{
		if (!isDead)
			isDead = true;
		return;
	}

	//direction vector for aiming at player
	b2Vec2 aim = playerPos - m_body->GetWorldCenter();
	//if player is too far away make sure enemy wont shoot
	if (aim.Length() > maxShootLength)
		willShoot = false;
	else
		willShoot = true;

	//normailise aim vectore to set force of shot
	aim.Normalize();
	b2Vec2 force(aim.x * shotForce * fpsScale, aim.y * shotForce * fpsScale);

	//increase timer. if time reached change direction and shoot if player is close
	changeTimer += fpsScale;
	if (changeTimer >= changeAt)
	{
		changeTimer = 0;
		direction *= -1;
		//enemies of type 3 do a small jump
		m_body->ApplyForceToCenter(b2Vec2(0, 500), true);
		if (willShoot)
			Shoot(force);
	}
	//update enemy movement depending on type
	switch (enemyType)
	{
		//type 3 stays on bottom floor even when gravity changes
		case 3:
			if (gravity.y > 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, -b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed * fpsScale, m_body->GetLinearVelocity().y));
			break;
		//type 4 stays on left wall
		case 4:
			if (gravity.x > 0)
				m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 0), true);
			else if (gravity.x == 0)
				m_body->ApplyForceToCenter(b2Vec2(-b2Abs(gravity.y) * m_body->GetMass(), -gravity.y), true);
			m_body->SetLinearVelocity(b2Vec2(m_body->GetLinearVelocity().x, direction * speed * fpsScale));
			break;
		//stays on ceiling
		case 5:
			if (gravity.y < 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed * fpsScale, m_body->GetLinearVelocity().y));
			break;
		//right wall
		case 6:
			if (gravity.x < 0)
				m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 0), true);
			else if (gravity.x == 0)
				m_body->ApplyForceToCenter(b2Vec2(b2Abs(gravity.y) * m_body->GetMass(), -gravity.y), true);
			m_body->SetLinearVelocity(b2Vec2(m_body->GetLinearVelocity().x, direction * speed * fpsScale));
			break;

		default:
			if (gravity.y > 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, -b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed * fpsScale, m_body->GetLinearVelocity().y));
			break;
			
	}

	UpdateFromSimulation(m_body);

	//updates bullet and checks if it can currently be shot
	bullet->Update();
	if (!bullet->GetBody()->IsActive())
	{
		canShoot = true;
	}
}


gef::MeshInstance* Enemy::GetBulletMesh()
{
	return bullet;
}

bool Enemy::GetCanShoot()
{
	return canShoot;
}

void Enemy::Shoot(b2Vec2 force)
{
	//get direction vector from force to offset the shot from the enemy body
	b2Vec2 aim = force;
	aim.Normalize();
	if (canShoot)
	{
		//play shoot sound effect
		voice_id_shoot = m_audioManager->PlaySample(sfx_id_shoot);
		canShoot = false;
		bullet->Fire(force, m_body->GetPosition(), b2Vec2(aim.x * 1.5f, aim.y * 1.5f));
	}
}

b2Vec2 Enemy::GetPosition()
{
	return m_body->GetPosition();
}


Enemy::~Enemy()
{
	canShoot = true;
	if (voice_id_shoot != -1)
		m_audioManager->StopPlayingSampleVoice(voice_id_shoot);
	if (voice_id_move != -1)
		m_audioManager->StopPlayingSampleVoice(voice_id_shoot);
	m_world->DestroyBody(m_body);
	m_body = NULL;
	delete mesh_;
	mesh_ = NULL;
	m_audioManager = NULL;
	bullet = NULL;
}
