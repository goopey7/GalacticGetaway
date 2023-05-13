#include "Bullet.h"

#include "system/debug_log.h"

Bullet::Bullet()
{
	tag = Tag::Bullet;
}

void Bullet::Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target, float speed)
{
	target_ = target;
	damage_ = damage;
	speed_ = speed;
	b2Vec2 b2_target_vector(target_vector.x * speed, -target_vector.y * speed);
	b2Vec2 b2_start_pos(start_pos.x, start_pos.y);
	setAlive(true);
	physics_body_->SetEnabled(true);
	physics_body_->SetTransform(b2_start_pos, 0);
	//physics_body_->ApplyLinearImpulseToCenter(b2_target_vector, true);
	physics_body_->SetLinearVelocity({b2_target_vector.x, b2_target_vector.y});
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
	if(other->GetTag() == Tag::Bullet)
	{
		return;
	}
	
	if(target_ == Tag::Player && other->GetTag() == Tag::Player)
	{
		Kill();
	}
	else if(target_ == Tag::Enemy && other->GetTag() == Tag::Enemy)
	{
		Kill();
	}
	else if(other->GetTag() != Tag::Player && other->GetTag() != Tag::Enemy)
	{
		Kill();
	}
}

GameObject::Tag Bullet::getTarget() const
{
	return target_;
}
