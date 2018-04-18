#include "Enemy.h"



Enemy::Enemy(int type, b2World* world, PrimitiveBuilder* builder) :
	m_world(world),
	m_builder(builder),
	bullet(NULL)
{
	enemyType = type;
	Init();
	CreateBullet();
	canShoot = true;
}

void Enemy::Init()
{
	if (enemyType > 3)
		enemyType = 3;
	else if (enemyType < 0)
		enemyType = 0;
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
	enemy_body_def.position = b2Vec2(10, 6);

	m_body = m_world->CreateBody(&enemy_body_def);

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

void Enemy::CreateBullet()
{
	bullet = new Bullet(m_body->GetPosition(), m_world, m_builder);
	canShoot = true;
}

void Enemy::Update(b2Vec2 gravity)
{
	//do with fps
	changeTimer++;
	if (changeTimer >= changeAt)
	{
		changeTimer = 0;
		direction *= -1;
		m_body->ApplyForceToCenter(b2Vec2(0, 500), true);
	}
	switch (enemyType)
	{
		case 0:
			if (gravity.y > 0)
				m_body->ApplyForceToCenter(b2Vec2(0, -2 * gravity.y * m_body->GetMass()), true);
			else if (gravity.y == 0)
				m_body->ApplyForceToCenter(b2Vec2(-gravity.x, -b2Abs(gravity.x) * m_body->GetMass()), true);
			m_body->SetLinearVelocity(b2Vec2(direction * speed, m_body->GetLinearVelocity().y));
			break;

		case 1:
			m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 2 * gravity.y * m_body->GetMass()), true);
			break;
			
		case 2:
			m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 2 * gravity.y * m_body->GetMass()), true);
			break;

		case 3:
			m_body->ApplyForceToCenter(b2Vec2(-2 * gravity.x * m_body->GetMass(), 2 * gravity.y * m_body->GetMass()), true);
			break;
			
	}

	UpdateFromSimulation(m_body);


	if (bullet != NULL)
	{
		bullet->Update();
		if (bullet->GetBody()->IsAwake())
		{
			m_body->SetLinearVelocity(b2Vec2_zero);
			if (bullet->GetBody()->GetContactList() != NULL)
			{
				bullet->Reset(GetPosition());
				canShoot = true;
			}
		}
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
		bullet->Fire(b2Vec2(0, 100), m_body->GetPosition(), b2Vec2(0, 1.5f));
		canShoot = false;
	}
}

b2Vec2 Enemy::GetPosition()
{
	return m_body->GetPosition();
}


Enemy::~Enemy()
{
}
