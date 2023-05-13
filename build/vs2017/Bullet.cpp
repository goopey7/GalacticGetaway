#include "Bullet.h"

#include "system/debug_log.h"

Bullet::Bullet()
{
	tag = Tag::Bullet;
}

void Bullet::Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target, float speed) {
	target_ = target;
	damage_ = damage;
	b2Vec2 b2_target_vector(target_vector.x * speed_, -target_vector.y * speed_);
	b2Vec2 b2_start_pos(start_pos.x, start_pos.y);
	setAlive(true);
	physics_body_->SetEnabled(true);
	physics_body_->SetTransform(b2_start_pos, 0);
	physics_body_->ApplyLinearImpulseToCenter(b2_target_vector, true);
	EnableCollisionResolution(bCollisionEnabled);
}

void Bullet::Update()
{
	EnableCollisionResolution(bCollisionEnabled);
	GameObject::Update();
}

void Bullet::PreResolve(GameObject* other)
{
}

void Bullet::EndCollision(GameObject* other)
{
	if(other->GetTag() != Tag::Player)
	{
		damage_ = 0;
	}
}

void Bullet::PostResolve(GameObject* other)
{
}

void Bullet::BeginCollision(GameObject* other)
{
	if(other != nullptr && other->GetTag() == Tag::Bullet)
	{
		bCollisionEnabled = false;
	}
	else
	{
		bCollisionEnabled = true;
	}
}

GameObject::Tag Bullet::getTarget() const
{
	return target_;
}
