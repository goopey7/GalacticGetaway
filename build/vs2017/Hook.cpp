#include "Hook.h"

#include "system/debug_log.h"

Hook::Hook(b2World* world, PrimitiveBuilder* builder)
{
	world_ = world;
	builder_ = builder;
	GameObject::Init(0.7f, 0.7f, 0.7f, 0, 0, world_, builder_, true);
	color_ = &builder_->green_material();
	tag = Tag::Hook;
}

void Hook::Update()
{
	GameObject::Update();
}

void Hook::Fire(gef::Vector2 target_vector, gef::Vector2 start_pos)
{
	b2Vec2 b2_target_vector(target_vector.x * 10.f, -target_vector.y * 10.f);
	b2Vec2 b2_start_pos(start_pos.x, start_pos.y);
	physics_body_->SetEnabled(true);
	physics_body_->SetTransform(b2_start_pos, 0);
	physics_body_->ApplyLinearImpulseToCenter(b2_target_vector, true);
}

void Hook::BeginCollision(GameObject* other)
{
	gef::DebugOut("Hook::BeginCollision\n");
	if(other != nullptr)
	{
		if(other->GetTag() == Player)
		{
			color_ = &builder_->blue_material();
		}
		if(other->GetTag() == Bullet)
		{
			color_ = &builder_->red_material();
		}
	}
}

void Hook::EndCollision(GameObject* other)
{
	gef::DebugOut("Hook::EndCollision\n");
	color_ = &builder_->green_material();
}

const gef::Material* Hook::GetColor()
{
	return color_;
}

void Hook::SetColor(gef::Material* color)
{
	color_ = color;
}
