#include "game_object.h"
#include <system/debug_log.h>




GameObject::GameObject()
{
	SetType(NONE);
	SetBulletType(NOBULLET);
}
//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation(const b2Body* body)
{
	if (body)
	{
		// setup object rotation
		gef::Matrix44 object_rotation;
		object_rotation.RotationZ(body->GetAngle());

		// setup the object translation
		gef::Vector4 object_translation(body->GetPosition().x, body->GetPosition().y, 0.0f);

		// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation;
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}


void GameObject::MyCollisionResponse()
{
	gef::DebugOut("No type\n");
}

void GameObject::SetType(ObjectType type)
{
	type_member = type;
}

ObjectType GameObject::GetType()
{
	if (type_member!= NULL && type_member > 0)
		return type_member;
	else
		return NONE;
}

void GameObject::SetBulletType(BulletType value)
{
	bulletType = value;
}

BulletType GameObject::GetBulletType()
{
	return bulletType;
}

