#include "Player.h"



Player::Player(b2World* world, PrimitiveBuilder* builder, b2Vec2 startPos, gef::AudioManager* audioManager, int shootID, int moveID) :
	m_world(world),
	m_builder(builder),
	bullet(NULL)
{
	startPosition = startPos;
	InitBody();
	sfx_id_shoot = shootID;
	sfx_id_move = moveID;
	voice_id_shoot = -1;
	voice_id_move = -1;
	m_audioManager = audioManager;
	CreateBullet();
	CreateTurret();
	canShoot = true;
	playerRight = false;
	playerLeft = false;
	maxHealth = 5;
	health = maxHealth;
}

void Player::InitBody()
{
	speed = 6.0f;

	gef::Vector4 halfDimensions(0.75f, 0.75f, 0.75f);
	set_mesh(m_builder->CreateBoxMesh(halfDimensions));
	
	

	// create a physics body for the player
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.fixedRotation = true;
	player_body_def.position = startPosition;

	m_body = m_world->CreateBody(&player_body_def);
	m_body->SetUserData(this);

	// create the shape for the player
	b2PolygonShape player_shape;
	player_shape.SetAsBox(0.75f, 0.75f);

	// create the fixture
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;
	// create the fixture on the rigid body
	m_body->CreateFixture(&player_fixture_def);
	//m_body->CreateFixture(&turret_fixture_def);

	//set player gamebject type

	SetType(PLAYER);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);


}

void Player::CreateBullet()
{
	bullet = new Bullet(m_body->GetPosition(), m_world, m_builder);
	bullet->SetBulletType(PLAYERBULLET);
	canShoot = true;
}

void Player::CreateTurret()
{
	turretRot = 0;
	gef::Vector4 halfSize(0.125f, 0.5f, 0);
	turret = new GameObject();
	turret->set_mesh(m_builder->CreateBoxMesh(halfSize));

	PositionTurret(0, 1);
	turret->SetType(NONE);
}

void Player::PositionTurret(float x, float y)
{
	b2Vec2 norms(x, y);
	norms.Normalize();
	// setup object rotation
	gef::Matrix44 object_rotation;
	object_rotation.RotationZ(turretRot);

	// setup the object translation
	gef::Vector4 object_translation(m_body->GetPosition().x + norms.x, m_body->GetPosition().y - norms.y, 0.0f);

	// build object transformation matrix
	gef::Matrix44 object_transform = object_rotation;
	object_transform.SetTranslation(object_translation);
	turret->set_transform(object_transform);
}



void Player::Update(float x, float y, float frame_time)
{
	m_frameTime = frame_time;
	UpdateFromSimulation(m_body);
	bullet->Update();
	if (!bullet->GetBody()->IsActive())
	{
		canShoot = true;
	}
	stickX = x;
	stickY = y;
	turretRot = b2Atan2(stickX, stickY);;
	PositionTurret(stickX, stickY);

	
	//turret->UpdateFromSimulation(m_turretBody);
	//if (bullet != NULL && !canShoot)
	//{
	//	if (bullet->GetBody()->IsActive())
	//	{
	//		if (bullet->GetBody()->GetContactList() != NULL)
	//		{
	//			//bullet->Reset();
	//			canShoot = true;
	//		}
	//	}
	//}
}
int Player::GetHealth()
{
	return health;
}

int Player::GetMaxHealth()
{
	return maxHealth;
}

void Player::ReduceHealth()
{
	health--;
}

void Player::SetHealth(int value)
{
	health = value;
}

void Player::ResetPlayer(b2Vec2 startPos)
{
	SetHealth(maxHealth);
	SetPosition(startPos);
	bullet->GetBody()->SetActive(false);
}

GameObject* Player::GetTurret()
{
	return turret;
}

void Player::SetTurretAngle(float radians)
{
	m_turretBody->SetTransform(m_turretBody->GetPosition(), radians);
}

gef::MeshInstance* Player::GetBulletMesh()
{
	return bullet;
}

bool Player::GetCanShoot()
{
	return canShoot;
}

void Player::SetCanShoot(bool value)
{
	canShoot = value;
}

void Player::Shoot(b2Vec2 force)
{
	if (canShoot)
	{
		voice_id_shoot = m_audioManager->PlaySample(sfx_id_shoot);
		b2Vec2 norms(stickX, stickY);
		norms.Normalize();
		bullet->Fire(force, m_body->GetPosition(), b2Vec2(norms.x * 1.5f, -norms.y * 1.5f));
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

void Player::SetMoveSound(bool play)
{
	if (play && voice_id_move == -1)
	{
		voice_id_move = m_audioManager->PlaySample(sfx_id_move, true);
	}
	else if (!play && voice_id_move != -1)
	{
		m_audioManager->StopPlayingSampleVoice(voice_id_move);
		voice_id_move = -1;
	}
}

Player::~Player()
{
	delete turret;
	turret = NULL;
	bullet = NULL;
}
