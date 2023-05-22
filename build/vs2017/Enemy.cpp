#include "Enemy.h"

#include <d3d10.h>

#include "Bullet.h"
#include "Player.h"
#include "system/debug_log.h"

void Enemy::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world,
	SpriteAnimator3D* sprite_animator, const ::Player* player)
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
}

void Enemy::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, SpriteAnimator3D* sprite_animator, const ::Player* player)
{
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, sprite_animator, player);
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

	/*fire_timer_ += frame_time;
	bullet_manager_.Update();*/

	// Raycast to check if player is in range
	b2Vec2 cast_start = GetBody()->GetPosition();
	//b2Vec2 cast_end = cast_start + (moving_left_ ? -1 : 1)*b2Vec2(player_detection_range_, 0.f);
	b2Vec2 direction = player_->GetBody()->GetPosition() - GetBody()->GetPosition();
	direction.Normalize();
	b2Vec2 cast_end = cast_start +  b2Vec2(direction.x * player_detection_range_, direction.y * player_detection_range_);
	bPlayerInRange_ = false;
	physics_world_->RayCast(this, cast_start, cast_end);
	
	if(!bSawPlayer || !bPlayerInRange_)
	{
		// TODO Movement depending on gravity. Maybe move like red Koopas?
		animation_state_ = RUNNING;
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
		gun_.SetTargetVector({ moving_left_ ? -1.f : 1.f, 0 });
		gun_.Update(frame_time, transform().GetTranslation());
	}
	else
	{
		animation_state_ = IDLE;
		//b2Vec2 pos = GetBody()->GetPosition() + b2Vec2(0.f, size_y_ / 8.f);
		//b2Vec2 dir = player_->GetBody()->GetPosition() - pos;
		b2Vec2 dir = player_->GetBody()->GetPosition() - GetBody()->GetPosition();
		gun_.SetTargetVector({ dir.x,-dir.y });
		gun_.Update(frame_time, transform().GetTranslation());

		gun_.Fire(frame_time, GameObject::Tag::Player);
		//dir.Normalize();
		/*if(fire_timer_ >= fire_rate_)
		{
			bullet_manager_.Fire({dir.x,-dir.y}, {pos.x, pos.y}, damage_, GameObject::Tag::Player, 20.f);
			fire_timer_ = 0.f;
		}*/
	}

	UpdateBox2d();

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
	renderer_3d->DrawMesh(*this);
	gun_.Render(renderer_3d);
	//bullet_manager_.Render(renderer_3d);
}
