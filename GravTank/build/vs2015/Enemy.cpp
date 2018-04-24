#include "Enemy.h"



Enemy::Enemy(int type, b2World* world, PrimitiveBuilder* builder, b2Vec2 start) :
	m_world(world),
	m_builder(builder),
	bullet(NULL)
{
	enemyType = type;
	startPosition = start;
	Init();
	CreateBullet();
	canShoot = true;
	isDead = false;
}

void Enemy::Init()
{
	if (enemyType > 6)
		enemyType = 6;
	else if (enemyType < 3)
		enemyType = 3;
	changeTimer = 0;
	changeAt = 160;
	speed = 3.0f;
	direction = 1;

	// setup the mesh for the Enemy
	set_mesh(m_builder->CreateSphereMesh(1.0f, 10, 10));

	// create a physics body for the player
	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;
	enemy_body_def.fixedRotation = false;
	enemy_body_def.position = startPosition;

	m_body = m_world->CreateBody(&enemy_body_def);
	m_body->SetUserData(this);

	// create the shape for the player
	b2CircleShape enemy_shape;
	enemy_shape.m_p.SetZero();
	enemy_shape.m_radius = 1;

	// create the fixture
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	m_body->CreateFixture(&enemy_fixture_def);

	//set player gamebject type
	SetType(ENEMY);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);
}

void Enemy::Die()
{
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

void Enemy::Update(b2Vec2 gravity)
{
	if (!m_body->IsActive())
	{
		if (!isDead)
			isDead = true;
		return;
	}
	//do with fps
	changeTimer++;
	if (changeTimer >= changeAt)
	{
		changeTimer = 0;
		direction *= -1;
		m_body->ApplyForceToCenter(b2Vec2(0, 500), true);
		Shoot(b2Vec2(0, 150));
	}
	switch (enemyType)
	{
		case 3:
			if (gravity.y > 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, -b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed, m_body->GetLinearVelocity().y));
			break;

		case 4:
			if (gravity.x > 0)
				m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 0), true);
			else if (gravity.x == 0)
				m_body->ApplyForceToCenter(b2Vec2(-b2Abs(gravity.y) * m_body->GetMass(), -gravity.y), true);
			m_body->SetLinearVelocity(b2Vec2(m_body->GetLinearVelocity().x, direction * speed));
			break;
			
		case 5:
			if (gravity.y < 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed, m_body->GetLinearVelocity().y)); 
			break;

		case 6:
			if (gravity.x < 0)
				m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 0), true);
			else if (gravity.x == 0)
				m_body->ApplyForceToCenter(b2Vec2(b2Abs(gravity.y) * m_body->GetMass(), -gravity.y), true);
			m_body->SetLinearVelocity(b2Vec2(m_body->GetLinearVelocity().x, direction * speed)); 
			break;

		default:
			if (gravity.y > 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, -b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed, m_body->GetLinearVelocity().y));
			break;
			
	}

	UpdateFromSimulation(m_body);

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
	if (canShoot)
	{
		canShoot = false;
		bullet->Fire(force, m_body->GetPosition(), b2Vec2(0, 1.5f));
	}
}

b2Vec2 Enemy::GetPosition()
{
	return m_body->GetPosition();
}


Enemy::~Enemy()
{
	m_world->DestroyBody(m_body);
	m_body = NULL;
	delete mesh_;
	mesh_ = NULL;
}
