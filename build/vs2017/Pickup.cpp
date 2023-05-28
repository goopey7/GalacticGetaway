#include "Pickup.h"

Pickup::Pickup()
{
	tag = Tag::Pickup;
}

void Pickup::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world,
	PrimitiveBuilder* builder, bool dynamic)
{
	GameObject::Init(size_x, size_y, size_z, pos_x, pos_y, world, builder, dynamic);
	EnableCollisionResolution(false);
	GetBody()->SetGravityScale(0.f);
}

void Pickup::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, bool dynamic)
{
	Init(size, pos, world, builder, dynamic);
}

void Pickup::SetTargetBody(b2Body* target_body)
{
	target_body_ = target_body;
}

void Pickup::Update(float frame_time)
{
	if(is_active_)
	{
		GameObject::Update(frame_time);
	}
	else
	{
		GetBody()->SetTransform(target_body_->GetPosition(), 0.f);
	}
}

void Pickup::Render(gef::Renderer3D* renderer_3d) const
{
	if(is_active_)
		GameObject::Render(renderer_3d);
}

void Pickup::BeginCollision(GameObject* other)
{
		GameObject::BeginCollision(other);
}

void Pickup::Activate()
{
	is_active_ = true;
}
