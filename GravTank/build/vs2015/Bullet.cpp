#include "Bullet.h"



Bullet::Bullet(b2Vec2 pos, b2World* world, PrimitiveBuilder* builder) :
	m_world(world),
	m_builder(builder)
{
	// setup the mesh
	set_mesh(m_builder->CreateSphereMesh(0.2f, 10, 10));

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.fixedRotation = false;
	body_def.position = pos;

	m_body = m_world->CreateBody(&body_def);

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

	//set player gamebject type
	SetType(BULLET);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);
}

void Bullet::Update()
{
	UpdateFromSimulation(m_body);
}

void Bullet::Fire(b2Vec2 force, b2Vec2 pos, b2Vec2 offset)
{
	m_body->SetTransform(pos + offset, m_body->GetAngle());
	m_body->SetActive(true);

	m_body->SetLinearVelocity(b2Vec2_zero);
	m_body->ApplyForceToCenter(force, true);
}

void Bullet::Init()
{
	
}

b2Body* Bullet::GetBody()
{
	return m_body;
}


void Bullet::Reset(b2Vec2 pos)
{
	
	m_body->SetActive(false);
}

void Bullet::MyCollisionResponse()
{
	
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
	CleanUp();
}
