#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>

enum ObjectType
{
	NONE, PLAYER, ENEMY, BULLET
};

class GameObject : public gef::MeshInstance
{
public:
	GameObject();
	void UpdateFromSimulation(const b2Body* body);
	virtual void MyCollisionResponse();
	void SetType(ObjectType type);
	ObjectType GetType();

private:
	ObjectType type_member;
};

#endif // _GAME_OBJECT_H