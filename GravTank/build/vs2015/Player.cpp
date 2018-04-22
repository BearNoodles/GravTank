#include "Player.h"



Player::Player(b2World* world, PrimitiveBuilder* builder, b2Vec2 startPos) :
	m_world(world),
	m_builder(builder),
	bullet(NULL)
{
	startPosition = startPos;
	Init();
	CreateBullet();
	canShoot = true;
	playerRight = false;
	playerLeft = false;
}

void Player::Init()
{
	speed = 3.0f;

	// setup the mesh for the player
	set_mesh(m_builder ->GetDefaultCubeMesh());

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.fixedRotation = true;
	//player_body_def.position = levelBuilder->GetStartPosition();
	player_body_def.position = startPosition;

	m_body = m_world->CreateBody(&player_body_def);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(0.5f, 0.5f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	m_body->CreateFixture(&player_fixture_def);

	//set player gamebject type
	SetType(PLAYER);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);
}

void Player::CreateBullet()
{
	bullet = new Bullet(m_body->GetPosition(), m_world, m_builder);
	canShoot = true;
}

void Player::Update()
{
	UpdateFromSimulation(m_body);

	if (bullet != NULL && !canShoot)
	{
		if (bullet->GetBody()->IsAwake())
		{
			if (bullet->GetBody()->GetContactList() != NULL)
			{
				bullet->Reset(GetPosition());
				canShoot = true;
			}


			bullet->Update();
		}
	}
}

gef::MeshInstance* Player::GetBulletMesh()
{
	return bullet;
}

bool Player::GetCanShoot()
{
	return canShoot;
}

void Player::Shoot(b2Vec2 force)
{
	if (canShoot)
	{
		bullet->Fire(force, m_body->GetPosition(), b2Vec2(0, 1.5f));
		canShoot = false;
	}
}

b2Vec2 Player::GetPosition()
{
	return m_body->GetPosition();
}

void Player::SetPosition(b2Vec2 pos)
{
	m_body->SetTransform(pos, m_body->GetAngle());
}

b2Vec2 Player::GetVelocity()
{
	return m_body->GetLinearVelocity();
}

void Player::SetVelocity(b2Vec2 vel)
{
	m_body->SetLinearVelocity(vel);
}

bool Player::GetPlayerRight()
{
	return playerRight;
}

bool Player::GetPlayerLeft()
{
	return playerLeft;
}

void Player::SetPlayerRight(bool value)
{
	playerRight = value;
}

void Player::SetPlayerLeft(bool value)
{
	playerLeft = value;
}

Player::~Player()
{
}