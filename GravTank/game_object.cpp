#include "game_object.h"
#include <system/debug_log.h>




GameObject::GameObject()
{
	SetType(NONE);
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
	if (type_member == PLAYER)
		gef::DebugOut("I am a Player\n");
	else if (type_member == ENEMY)
		gef::DebugOut("I am an Enemy\n");
	else
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

