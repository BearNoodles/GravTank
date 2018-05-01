#include "Bullet.h"



Bullet::Bullet(b2Vec2 pos, b2World* world, PrimitiveBuilder* builder) :
	m_world(world),
	m_builder(builder)
{
	SetBulletType(NOBULLET);
	// setup the mesh
	set_mesh(m_builder->CreateSphereMesh(0.2f, 10, 10));

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.fixedRotation = false;
	body_def.position = pos;

	m_body = m_world->CreateBody(&body_def);
	m_body->SetUserData((GameObject*)this);

	// create the shape
	b2CircleShape shape;
	shape.m_p.SetZero();
	shape.m_radius = 0.1f;

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = 10.0f;

	// create the fixture on the rigid body
	m_body->CreateFixture(&fixture_def);

	//set bullet gamebject type
	SetType(BULLET);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);

	//start body as inactive
	m_body->SetActive(false);
}

void Bullet::Update()
{
	//update position of bullet from body if it is currently active
	if (m_body->IsActive())
	{
		UpdateFromSimulation(m_body);
	}
}

void Bullet::Fire(b2Vec2 force, b2Vec2 pos, b2Vec2 offset)
{
	//set position of bullet to just in front of shooter
	m_body->SetTransform(pos + offset, m_body->GetAngle());
	//set body to active
	m_body->SetActive(true);

	//set velocity to zero in case is carries over from last shot
	m_body->SetLinearVelocity(b2Vec2_zero);
	//apply desired force to bullet
	m_body->ApplyForceToCenter(force, true);
}

void Bullet::Init()
{
	
}


b2Body* Bullet::GetBody()
{
	return m_body;
}


void Bullet::Reset()
{
	m_body->SetActive(false);
}

void Bullet::CleanUp()
{
	delete m_world;
	m_world = NULL;
	delete m_builder;
	m_builder = NULL;
}


Bullet::~Bullet()
{
	m_world->DestroyBody(m_body);
	m_body = NULL;
	delete mesh_;
	mesh_ = NULL;
	CleanUp();
}
