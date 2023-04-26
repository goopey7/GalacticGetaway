#include "CollisionManager.h"


#include "GameObject.h"
#include "box2d/b2_contact.h"
#include "system/debug_log.h"

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
	GameObject* a = (GameObject*) contact->GetFixtureA()->GetUserData().pointer;
	GameObject* b = (GameObject*) contact->GetFixtureB()->GetUserData().pointer;

	if(a != nullptr)
	{
		a->EndCollision(b);
	}

	if(b != nullptr)
	{
		b->EndCollision(a);
	}
}
