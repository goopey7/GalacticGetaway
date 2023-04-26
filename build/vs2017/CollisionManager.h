#pragma once
#include "box2d/b2_world_callbacks.h"

class CollisionManager : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;
	
};
