#include "Explosion.h"



Explosion::Explosion(b2Vec2 pos, b2World* world, PrimitiveBuilder* builder) :
	m_world(world),
	m_builder(builder)
{
	SetBulletType(NOBULLET);
	// setup the mesh

	gef::Vector4 halfDimensions(1.5f, 1.5f, 0.01f);
	set_mesh(m_builder->CreateBoxMesh(halfDimensions));

	// create a physics body
	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.fixedRotation = false;
	body_def.position = pos;

	m_body = m_world->CreateBody(&body_def);
	m_body->SetUserData((GameObject*)this);

	// create the shape
	b2PolygonShape shape;
	shape.SetAsBox(1.5f, 1.5f);

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

	m_body->SetActive(false);

	activeCount = 8;
	timer = 0;
}

void Explosion::Update()
{
	if (m_body->IsActive())
	{
		timer++;
		if (timer >= activeCount)
		{
			Reset();
		}
		m_body->SetLinearVelocity(b2Vec2_zero);
		UpdateFromSimulation(m_body);
	}
}

void Explosion::Activate(b2Vec2 pos)
{
	timer = 0;
	m_body->SetTransform(pos, m_body->GetAngle());
	m_body->SetActive(true);

	m_body->SetLinearVelocity(b2Vec2_zero);
}

bool Explosion::IsActive()
{
	return m_body->IsActive();
}

void Explosion::Init()
{

}


b2Body* Explosion::GetBody()
{
	return m_body;
}


void Explosion::Reset()
{
	m_body->SetActive(false);
}


void Explosion::CleanUp()
{
	delete m_world;
	m_world = NULL;
	delete m_builder;
	m_builder = NULL;
}


Explosion::~Explosion()
{
	m_world->DestroyBody(m_body);
	m_body = NULL;
	delete mesh_;
	mesh_ = NULL;
	CleanUp();
}
