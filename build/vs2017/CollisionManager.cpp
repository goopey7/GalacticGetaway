#include "CollisionManager.h"


#include "GameObject.h"
#include "box2d/b2_contact.h"

void CollisionManager::BeginContact(b2Contact* contact)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

 	if(a != nullptr && !a->TimeToDie())
	{
		a->BeginCollision(b);
	}

 	if(b != nullptr && !b->TimeToDie())
	{
		b->BeginCollision(a);
	}

}

void CollisionManager::EndContact(b2Contact* contact)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

 	if(a != nullptr && !a->TimeToDie())
	{
		a->EndCollision(b);
	}

 	if(b != nullptr && !b->TimeToDie())
	{
		b->EndCollision(a);
	}
}

void CollisionManager::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
 	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

 	if(a != nullptr && !a->TimeToDie())
 	{
 		a->PreResolve(b);
 	}
 
 	if(b != nullptr && !b->TimeToDie())
 	{
 		b->PreResolve(a);
 	}
}

void CollisionManager::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
	auto* a = reinterpret_cast<GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
  	auto* b = reinterpret_cast<GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

 	if(a != nullptr && !a->TimeToDie())
  	{
  		a->PostResolve(b);
  	}
  
 	if(b != nullptr && !b->TimeToDie())
  	{
  		b->PostResolve(a);
  	}
}
