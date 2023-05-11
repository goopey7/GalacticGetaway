#include "Bullet.h"

Bullet::Bullet()
{
	tag = Tag::Bullet;
}

void Bullet::Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target) {
	target_ = target;
	damage_ = damage;
	b2Vec2 b2_target_vector(target_vector.x * 10, -target_vector.y * 10);
	b2Vec2 b2_start_pos(start_pos.x, start_pos.y);
	setAlive(true);
	physics_body_->SetEnabled(true);
	physics_body_->SetTransform(b2_start_pos, 0);
	physics_body_->ApplyLinearImpulseToCenter(b2_target_vector, true);
}

void Bullet::EndCollision(GameObject* other)
{
	if(other->GetTag() != Tag::Player)
	{
		damage_ = 0;
	}
}

GameObject::Tag Bullet::getTarget() const
{
	return target_;
}
