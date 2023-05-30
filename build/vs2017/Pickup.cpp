#include "Pickup.h"
#include <cmath>
#include "Player.h"
#include "audio/audio_manager.h"

Pickup::Pickup()
{
	tag = Tag::Pickup;
}

void Pickup::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world,
				PrimitiveBuilder* builder, gef::AudioManager* am, bool dynamic)
{
	audio_manager_ = am;
	//size_ = gef::Vector4(size_x, size_y, size_z);
	//set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));

	b2BodyDef body_def;
	dynamic ? body_def.type = b2_dynamicBody : body_def.type = b2_staticBody;
	body_def.position = b2Vec2(pos_x, pos_y);
	body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

	b2PolygonShape shape;
	shape.SetAsBox(mesh_->aabb().max_vtx().x(), mesh_->aabb().max_vtx().y());

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.7f;
	fixture.userData.pointer = reinterpret_cast<uintptr_t>(this);

	physics_body_ = world->CreateBody(&body_def);
	physics_body_->CreateFixture(&fixture);

	UpdateBox2d();
	
	EnableCollisionResolution(false);
	GetBody()->SetGravityScale(0.f);

}

void Pickup::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, gef::AudioManager* am, bool dynamic)
{
	Init(size, pos, world, builder, am, dynamic);
}

void Pickup::SetTargetBody(b2Body* target_body)
{
	target_body_ = target_body;
}

void Pickup::Update(float frame_time)
{
	if(is_active_)
	{
		bobbing_time_ += frame_time;
		GetBody()->SetTransform(b2Vec2(start_pos_.x, start_pos_.y + 0.5*sin(bobbing_time_*3)), 0.f);
		UpdateBox2d();
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
	if(is_active_ && other->GetTag() == Tag::Player)
	{
		auto* player = dynamic_cast<Player*>(other);
		if(type_ == MaxAmmo)
		{
			player->GetGun()->addAmmo();
			if(player->GetGun()->isMax()) audio_manager_->PlaySample(1);
			else audio_manager_->PlaySample(0);
		}
		else if(type_ == Health)
		{
			player->Heal(3);
			audio_manager_->PlaySample(0);
		}
		Kill();
	}
}

void Pickup::SetType(Type type)
{
	type_ = type;
}

void Pickup::Activate()
{
	is_active_ = true;
	start_pos_ = GetBody()->GetTransform().p;
}

void Pickup::Activate(Type type)
{
	type_ = type;
	is_active_ = true;
}
