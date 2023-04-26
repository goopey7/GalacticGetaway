#include "CollisionManager.h"


#include "GameObject.h"
#include "box2d/b2_contact.h"

void CollisionManager::BeginContact(b2Contact* contact)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

	if(a != nullptr)
	{
		a->BeginCollision(b);
	}

	if(b != nullptr)
	{
		b->BeginCollision(a);
	}

}

void CollisionManager::EndContact(b2Contact* contact)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

	if(a != nullptr)
	{
		a->EndCollision(b);
	}

	if(b != nullptr)
	{
		b->EndCollision(a);
	}
}

void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
 	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);
 
 	if(a != nullptr)
 	{
 		a->PreResolve(b);
 	}
 
 	if(b != nullptr)
 	{
 		b->PreResolve(a);
 	}
}

void CollisionManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
  	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);
  
  	if(a != nullptr)
  	{
  		a->PostResolve(b);
  	}
  
  	if(b != nullptr)
  	{
  		b->PostResolve(a);
  	}
}
