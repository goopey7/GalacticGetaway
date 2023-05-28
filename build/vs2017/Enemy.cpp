#include "Enemy.h"

#include <d3d10.h>

#include "Bullet.h"
#include "Player.h"
#include <maths/math_utils.h>

#include "Pickup.h"
#include "system/debug_log.h"

void Enemy::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world,
				PrimitiveBuilder* builder, SpriteAnimator3D* sprite_animator, const Player* player, std::vector<GameObject*>&
				dynamic_game_objects)
{
	size_y_ = size_y;
	player_ = player;
	tag = Tag::Enemy;
	platform_ = sprite_animator->GetPlatform();
	sprite_animator3D_ = sprite_animator;
	set_mesh(sprite_animator3D_->GetFirstFrame("EnemyIdle"));

	physics_world_ = world;

	gun_.Init(gef::Vector4(size_x * 0.4f, size_y, size_z), world, sprite_animator, "Enemy/Gun/gun.png");

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

	dynamic_game_objects_ = &dynamic_game_objects;
	primitive_builder_ = builder;

	//TODO probablility of spawning a pickup
	auto pos = GetBody()->GetPosition();
	pickup_ = new Pickup();
	pickup_->Init(0.3,0.3,0.3, pos.x, pos.y, physics_world_, primitive_builder_, true);
	pickup_->SetTargetBody(GetBody());
	dynamic_game_objects_->push_back(pickup_);
}

void Enemy::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, SpriteAnimator3D* sprite_animator, const
				Player* player, std::vector<GameObject*>& dynamic_game_objects)
{
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, builder, sprite_animator, player, dynamic_game_objects);
}

void Enemy::Update(float frame_time)
{
	world_gravity_ = physics_world_->GetGravity();
	if(world_gravity_.y > 0)
	{
		world_gravity_direction_ = GravityDirection::GRAVITY_UP;
	}
	else if (world_gravity_.y < 0)
	{
		world_gravity_direction_ = GravityDirection::GRAVITY_DOWN;
	}
	else if (world_gravity_.x < 0)
	{
		world_gravity_direction_ = GravityDirection::GRAVITY_LEFT;
	}
	else if (world_gravity_.x > 0)
	{
		world_gravity_direction_ = GravityDirection::GRAVITY_RIGHT;
	}

	// Raycast to check if player is in range
	b2Vec2 cast_start = GetBody()->GetPosition();
	//b2Vec2 cast_end = cast_start + (moving_left_ ? -1 : 1)*b2Vec2(player_detection_range_, 0.f);
	b2Vec2 direction = player_->GetBody()->GetPosition() - GetBody()->GetPosition();
	direction.Normalize();
	b2Vec2 cast_end = cast_start +  b2Vec2(direction.x * player_detection_range_, direction.y * player_detection_range_);
	bPlayerInRange_ = false;
	closest_fraction_ = 1.f;
	closest_fixture_ = nullptr;
	physics_world_->RayCast(this, cast_start, cast_end);
	InspectClosestFixture();
	
	//if(!bSawPlayer || !bPlayerInRange_)
	if(!bPlayerInRange_)
	{
		// TODO Movement depending on gravity. Maybe move like red Koopas?
		animation_state_ = RUNNING;
		switch (world_gravity_direction_)
		{
		case GravityDirection::GRAVITY_UP:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2((moving_left_ ? -1.f : 1.f) * move_speed_ * frame_time, 0), gef::DegToRad(180));
			break;
		case GravityDirection::GRAVITY_DOWN:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2((moving_left_ ? -1.f : 1.f) * move_speed_ * frame_time, 0), 0);
			break;
		case GravityDirection::GRAVITY_LEFT:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, (moving_left_ ? 1.f : -1.f) * move_speed_ * frame_time), gef::DegToRad(-90));
			break;
		case GravityDirection::GRAVITY_RIGHT:
			//...
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, (moving_left_ ? -1.f : 1.f) * move_speed_ * frame_time), gef::DegToRad(90));
			break;
		}
		gun_.SetTargetVector({ moving_left_ ? -1.f : 1.f, 0 });
		gun_.Update(frame_time, transform().GetTranslation(), world_gravity_direction_);
	}
	else
	{
		animation_state_ = IDLE;
		b2Vec2 dir = player_->GetBody()->GetPosition() - GetBody()->GetPosition();
		gun_.SetTargetVector({ dir.x,-dir.y });
		gun_.Update(frame_time, transform().GetTranslation(), world_gravity_direction_);
		gun_.Fire(frame_time, GameObject::Tag::Player);
	}

	UpdateBox2d();

	if (moving_left_) Rotate(gef::Vector4(0, FRAMEWORK_PI, 0));
	else Rotate(gef::Vector4(0, 0, 0));
	anim_time_ += frame_time;
	switch (animation_state_)
	{
	case IDLE:
		set_mesh(sprite_animator3D_->UpdateAnimation(anim_time_, mesh_, "EnemyIdle"));
		break;
	case RUNNING:
		set_mesh(sprite_animator3D_->UpdateAnimation(anim_time_, mesh_, "EnemyRunning"));
		break;
	default:
		break;
	}
}

// On Raycast Hit
float Enemy::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
	if (fraction < closest_fraction_) {
		closest_fraction_ = fraction;
		closest_fixture_ = fixture;
	}
	return 1;
}

void Enemy::InspectClosestFixture() {
	if (closest_fixture_) {
		auto* object = reinterpret_cast<::GameObject*>(closest_fixture_->GetBody()->GetUserData().pointer);
		if (object != nullptr && object->GetTag() == Tag::Player)
		{
			auto* player = reinterpret_cast<::Player*>(object);
			if (player != nullptr)
			{
				if (closest_fraction_ <= 1.f)
				{
					bPlayerInRange_ = true;
					//bSawPlayer = true;
				}
			}
		}
	}
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
			
			if(health_ <= 0 && !dead)
			{
				// TODO select pickup type
				Pickup::Type type;
				type = Pickup::Type::MaxAmmo;
				pickup_->Activate(type);
				Kill();
			}
		}
	}
}

void Enemy::Render(gef::Renderer3D* renderer_3d) const
{
	renderer_3d->DrawMesh(*this);
	gun_.Render(renderer_3d);
	//bullet_manager_.Render(renderer_3d);
}
