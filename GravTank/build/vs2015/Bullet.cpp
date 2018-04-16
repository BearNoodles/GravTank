#include "Bullet.h"



Bullet::Bullet(b2Vec2 force, b2Vec2 pos, b2World* world, PrimitiveBuilder* builder) :
	m_world(world),
	m_builder(builder)
{
	SetType(BULLET);
	gef::Vector4 half_dimensions(0.2f, 0.2f, 0.5f);
	// setup the mesh
	mesh_ = m_builder->CreateBoxMesh(half_dimensions);

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.fixedRotation = false;
	body_def.position = pos;
	body_def.userData = this;

	m_body = m_world->CreateBody(&body_def);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(half_dimensions.x(), half_dimensions.y());

	// create the fixture
	b2FixtureDef fixture_def;
	fixture_def.shape = &shape;
	fixture_def.density = 1.0f;

	// create the fixture on the rigid body
	m_body->CreateFixture(&fixture_def);

	//set player gamebject type
	SetType(ENEMY);

	m_body->ApplyForceToCenter(force, true);

	// update visuals from simulation data
	UpdateFromSimulation(m_body);
}

void Bullet::Update()
{
	UpdateFromSimulation(m_body);
}

void Bullet::Init()
{
	
}


Bullet::~Bullet()
{
}
