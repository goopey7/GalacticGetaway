#pragma once
#include "box2d/b2_world_callbacks.h"

class CollisionManager : public b2ContactListener
{
public:
	
	// When entering the collision
	void BeginContact(b2Contact* contact) override;

	// When exiting the collision
	void EndContact(b2Contact* contact) override;

	// Before the collision is solved by box2d
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
	
	// After the collision is solved by box2d
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;
	
};
