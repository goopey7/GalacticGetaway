#include "Enemy.h"

#include <d3d10.h>

#include "Bullet.h"
#include "Player.h"
#include "system/debug_log.h"

void Enemy::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world,
				PrimitiveBuilder* builder, gef::Platform* platform, const ::Player* player)
{
	size_y_ = size_y;
	player_ = player;
	tag = Tag::Enemy;
	platform_ = platform;
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));

	physics_world_ = world;

	bullet_manager_.Init(world,builder);

	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2(pos_x, pos_y);
	body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

	b2PolygonShape shape;
	shape.SetAsBox(size_x, size_y);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.7f;
	fixture.userData.pointer = reinterpret_cast<uintptr_t>(this);

	physics_body_ = world->CreateBody(&body_def);
	physics_body_->CreateFixture(&fixture);
	physics_body_->SetSleepingAllowed(false);
	physics_body_->SetFixedRotation(true);

	UpdateBox2d();
}

void Enemy::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder,
	gef::Platform* platform, const ::Player* player)
{
	size_y_ = size.y();
	player_ = player;
	platform_ = platform;
	set_mesh(builder->CreateBoxMesh(size));
	
	physics_world_ = world;
	
	bullet_manager_.Init(world,builder);

	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2(pos.x(), pos.y());

	b2PolygonShape shape;
	shape.SetAsBox(size.x(), size.y());

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.5f;

	physics_body_ = world->CreateBody(&body_def);
	physics_body_->CreateFixture(&fixture);
	physics_body_->GetUserData().pointer = (uintptr_t)this;
	physics_body_->SetSleepingAllowed(false);
	physics_body_->SetFixedRotation(true);

	UpdateBox2d();
}

void Enemy::Update(float frame_time)
{
	world_gravity_ = physics_world_->GetGravity();
	if(std::abs(world_gravity_.y) > 0)
	{
		world_gravity_direction_ = GRAVITY_VERTICAL;
	}
	else if(world_gravity_.x < 0)
	{
		world_gravity_direction_ = GRAVITY_LEFT;
	}
	else if(world_gravity_.x > 0)
	{
		world_gravity_direction_ = GRAVITY_RIGHT;
	}

	fire_timer_ += frame_time;
	bullet_manager_.Update();

	// Raycast to check if player is in range
	b2Vec2 cast_start = GetBody()->GetPosition();
	b2Vec2 cast_end = cast_start + (moving_left_ ? -1 : 1)*b2Vec2(player_detection_range_, 0.f);
	bPlayerInRange_ = false;
	physics_world_->RayCast(this, cast_start, cast_end);
	
	if(!bSawPlayer)
	{
		// TODO Movement depending on gravity. Maybe move like red Koopas?

		switch (world_gravity_direction_)
		{
		case GRAVITY_VERTICAL:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2((moving_left_ ? -1.f : 1.f) * move_speed_ * frame_time, 0), 0);
			break;
		case GRAVITY_LEFT:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, (moving_left_ ? 1.f : -1.f) * move_speed_ * frame_time), 0);
			break;
		case GRAVITY_RIGHT:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, (moving_left_ ? -1.f : 1.f) * move_speed_ * frame_time), 0);
			break;
		}
	}
	else
	{
		b2Vec2 pos = GetBody()->GetPosition() + b2Vec2(0.f, size_y_ / 8.f);
		b2Vec2 dir = player_->GetBody()->GetPosition() - pos;
		dir.Normalize();
		if(fire_timer_ >= fire_rate_)
		{
			bullet_manager_.Fire({dir.x,dir.y}, {pos.x, pos.y}, damage_, GameObject::Tag::Player, 1000000000.f);
			fire_timer_ = 0.f;
		}
	}

	UpdateBox2d();
}

// On Raycast Hit
float Enemy::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
	auto* object = reinterpret_cast<::GameObject*>(fixture->GetBody()->GetUserData().pointer);
	if(object != nullptr && object->GetTag() == Tag::Player)
	{
		auto* player = reinterpret_cast<::Player*>(object);
		if(player != nullptr)
		{
			if(fraction <= 1.f)
			{
				bPlayerInRange_ = true;
				bSawPlayer = true;
			}
		}
	}

	return fraction;
}

void Enemy::BeginCollision(GameObject* other)
{
	if(other->GetTag() != Tag::Bullet)
	{
		moving_left_ = !moving_left_;
	}
	else
	{
		::Bullet* bullet = dynamic_cast<::Bullet*>(other);
		if(bullet->getTarget() == Tag::Enemy && bullet->getDamage() > 0)
		{
			health_ -= bullet->getDamage();
			bullet->setDamage(0);
			bullet->Kill();
			
			if(health_ <= 0)
			{
				Kill();
			}
		}
	}
}

void Enemy::Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const
{
	renderer_3d->set_override_material(&builder->red_material());
	renderer_3d->DrawMesh(*this);
	bullet_manager_.Render(renderer_3d);
}
