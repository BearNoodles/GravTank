#include "Player.h"



Player::Player(b2World* world, PrimitiveBuilder* builder, b2Vec2 startPos, gef::AudioManager* audioManager, int shootID, int moveID, int explodeID) :
	m_world(world),
	m_builder(builder),
	bullet(NULL),
	explode(NULL)
{
	//set start position
	startPosition = startPos;

	//give player a physics body
	InitBody();

	//set sound effect ids
	sfx_id_shoot = shootID;
	sfx_id_move = moveID;
	sfx_id_explode = explodeID;
	voice_id_shoot = -1;
	voice_id_move = -1;
	voice_id_explode = -1;
	m_audioManager = audioManager;
	
	//create bullet explosion and turret objects
	CreateBullet();
	CreateExplosion();
	CreateTurret();

	//playre can shoot from the start
	canShoot = true;

	//right and left have not been pressed
	playerRight = false;
	playerLeft = false;

	//set health initial values
	maxHealth = 6;
	health = maxHealth;

	//force which player fires bullet
	bulletForce = 200;
}

void Player::InitBody()
{
	//player speed
	speed = 9.0f;

	//mesh half dimensions
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

void Player::CreateExplosion()
{
	explode = new Explosion(m_body->GetPosition(), m_world, m_builder);
	explode->SetBulletType(EXPLOSION);
}

bool Player::IsExploding()
{
	return explode->IsActive();
}

void Player::CreateTurret()
{
	turretRot = 0;
	gef::Vector4 halfSize(0.125f, 0.75f, 0);
	turret = new GameObject();
	turret->set_mesh(m_builder->CreateBoxMesh(halfSize));

	PositionTurret(0, 1);
	turret->SetType(NONE);
}

void Player::PositionTurret(float x, float y)
{
	//normalised vectors for stick position, so turret doesnt gradually poke out
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



void Player::Update(float x, float y, b2Vec2 up)
{
	//update player body, and the bullet and explosion
	UpdateFromSimulation(m_body);
	bullet->Update();
	explode->Update();

	//check if bullet can be shot again
	if (!bullet->GetBody()->IsActive())
	{
		canShoot = true;
	}

	//change given stick input values for the current camera position
	if (up.x == 1)
	{
		stickX = -y;
		stickY = x; 
	}
	else if (up.y == -1)
	{
		stickX = -x;
		stickY = -y;
	}
	else if (up.x == -1)
	{
		stickX = y;
		stickY = -x;
	}
	else
	{
		stickX = x;
		stickY = y;
	}
	//float rot = b2Atan2(up.x, up.y);

	//set turret rotation based on sticks and up vector
	turretRot = b2Atan2(stickX, stickY);

	//update turret position
	PositionTurret(stickX, stickY);

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
	explode->GetBody()->SetActive(false);
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

gef::MeshInstance* Player::GetExplosionMesh()
{
	return explode;
}

bool Player::GetCanShoot()
{
	return canShoot;
}

void Player::SetCanShoot(bool value)
{
	canShoot = value;
}

void Player::Shoot(float shotScale)
{
	if (canShoot)
	{
		//play shot sound
		voice_id_shoot = m_audioManager->PlaySample(sfx_id_shoot);
		//get stick normals to use as an offset for the bullets start position so it doesnt touch the player
		b2Vec2 norms(stickX, stickY);
		norms.Normalize();
		bullet->Fire(b2Vec2(stickX * bulletForce * shotScale, -stickY * bulletForce * shotScale), m_body->GetPosition(), b2Vec2(norms.x * 1.5f, -norms.y * 1.5f));
		canShoot = false;
	}
}

void Player::ExplodeBullet()
{
	//if player cant currently shoot then the bullet is active meaning it can be exploded
	if (!canShoot)
	{
		voice_id_explode = m_audioManager->PlaySample(sfx_id_explode);
		explode->Activate(bullet->GetBody()->GetPosition());
		bullet->Reset();
		canShoot = true;
		
	}
}

b2Vec2 Player::GetPosition()
{
	return m_body->GetPosition();
}

void Player::SetPosition(b2Vec2 pos)
{
	//set body position (for resetting) and make sure the sound effect stops
	m_body->SetTransform(pos, m_body->GetAngle());
	if (voice_id_shoot != -1)
		m_audioManager->StopPlayingSampleVoice(voice_id_shoot);
	if (voice_id_move != -1)
		m_audioManager->StopPlayingSampleVoice(voice_id_move);
	if (voice_id_explode != -1)
		m_audioManager->StopPlayingSampleVoice(voice_id_explode);
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
