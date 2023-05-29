#include "Player.h"
#include <input/keyboard.h>
#include <maths/vector2.h>
#include <maths/math_utils.h>
#include "system/debug_log.h"
#include "Level.h"
#include "InputActionManager.h"

void Player::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, SpriteAnimator3D* sprite_animator, Camera* cam, Level* lev) {
	tag = Tag::Player;
	camera_ = cam;
	platform_ = sprite_animator->GetPlatform();
	level_ = lev;
	sprite_animator3D_ = sprite_animator;
	set_mesh(sprite_animator3D_->GetFirstFrame("PlayerIdle"));

	gun_.Init(gef::Vector4(size_x * 0.33f, size_y, size_z), world, sprite_animator, audio_manager_, "Player/Gun/gun.png");

	physics_world_ = world;

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

void Player::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, SpriteAnimator3D* sprite_animator, Camera* cam, Level* lev) {
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, sprite_animator, cam, lev);
}

void Player::Update(InputActionManager* iam, float frame_time) {
	if (animation_state_ != DEATH) {
		if (touching_end_object_) {
			if (iam->isPressed(UseItem)) level_->SetEndState(WIN);
		}

		// Movement
		if (iam->isHeld(MoveLeft)) {
			switch (player_gravity_direction_)
			{
			case GravityDirection::GRAVITY_UP:
			case GravityDirection::GRAVITY_DOWN:
				physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(-8 * frame_time, 0), physics_body_->GetAngle());
				break;
			case GravityDirection::GRAVITY_LEFT:
				physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, 8 * frame_time), physics_body_->GetAngle());
				break;
			case GravityDirection::GRAVITY_RIGHT:
				physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, -8 * frame_time), physics_body_->GetAngle());
				break;
			default:
				break;
			}
		}
		if (iam->isHeld(MoveRight)) {
			switch (player_gravity_direction_)
			{
			case GravityDirection::GRAVITY_UP:
			case GravityDirection::GRAVITY_DOWN:
				physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(8 * frame_time, 0), physics_body_->GetAngle());
				break;
			case GravityDirection::GRAVITY_LEFT:
				physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, -8 * frame_time), physics_body_->GetAngle());
				break;
			case GravityDirection::GRAVITY_RIGHT:
				physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, 8 * frame_time), physics_body_->GetAngle());
				break;
			default:
				break;
			}
		}

		if (iam->isPressed(GravityLock)) {
			gravity_lock_ = !gravity_lock_;
			if (gravity_lock_) physics_body_->SetGravityScale(0);
			else {
				physics_body_->SetGravityScale(1);
				b2Vec2 grav = world_gravity_;
				grav *= 0.01;
				physics_body_->ApplyLinearImpulseToCenter(grav, true);
				player_gravity_direction_ = world_gravity_direction_;
			}
		}

		if (iam->isPressed(Jump) && !gravity_lock_ && !jumping_) {
			jumping_ = true;
			animation_state_ = JUMPING;
			set_mesh(sprite_animator3D_->GetFirstFrame("PlayerJumping"));
			b2Vec2 grav = world_gravity_;
			grav *= 40;
			physics_body_->ApplyLinearImpulseToCenter(-grav, true);
		}

		if (iam->isPressed(GravityUp)) {
			camera_->Warp();
			world_gravity_ = b2Vec2(0, 1);
			physics_world_->SetAllowSleeping(false);
			world_gravity_direction_ = GravityDirection::GRAVITY_UP;
			if (!gravity_lock_) {
				player_gravity_direction_ = GravityDirection::GRAVITY_UP;
				camera_->SetAbovePlayer(false);
			}
		}
		else if (iam->isPressed(GravityDown)) {
			camera_->Warp();
			world_gravity_ = b2Vec2(0, -1);
			physics_world_->SetAllowSleeping(false);
			world_gravity_direction_ = GravityDirection::GRAVITY_DOWN;
			if (!gravity_lock_) {
				player_gravity_direction_ = GravityDirection::GRAVITY_DOWN;
				camera_->SetAbovePlayer(true);
			}
		}
		else if (iam->isPressed(GravityLeft)) {
			camera_->Warp();
			world_gravity_ = b2Vec2(-1, 0);
			physics_world_->SetAllowSleeping(false);
			world_gravity_direction_ = GravityDirection::GRAVITY_LEFT;
			if (!gravity_lock_) {
				player_gravity_direction_ = GravityDirection::GRAVITY_LEFT;
			}
		}
		else if (iam->isPressed(GravityRight)) {
			camera_->Warp();
			world_gravity_ = b2Vec2(1, 0);
			physics_world_->SetAllowSleeping(false);
			world_gravity_direction_ = GravityDirection::GRAVITY_RIGHT;
			if (!gravity_lock_) {
				player_gravity_direction_ = GravityDirection::GRAVITY_RIGHT;
			}
		}

		switch (player_gravity_direction_)
		{
		case GravityDirection::GRAVITY_UP:
			physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(180));
			break;
		case GravityDirection::GRAVITY_DOWN:
			physics_body_->SetTransform(physics_body_->GetPosition(), 0);
			break;
		case GravityDirection::GRAVITY_LEFT:
			physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(-90));
			break;
		case GravityDirection::GRAVITY_RIGHT:
			physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(90));
			break;
		default:
			break;
		}

		if (iam->isPressed(GravityStrenghtUp)) {
			camera_->Warp();
			grav_strength_changed_ = true;
			world_grav_mult = 50.f;
		}
		else if (iam->isPressed(GravityStrengthDown)) {
			camera_->Warp();
			grav_strength_changed_ = true;
			world_grav_mult = 0;
			b2Body* body_list = physics_world_->GetBodyList();
			while (body_list->GetNext()) {
				body_list->ApplyLinearImpulseToCenter(-world_gravity_, true);
				body_list = body_list->GetNext();
			}
		}

		if (grav_strength_changed_) {
			grav_strength_change_time += frame_time;
			if (grav_strength_change_time >= 5) {
				grav_strength_changed_ = false;
				grav_strength_change_time = 0;
				world_grav_mult = 10.f;
			}
		}

		b2Vec2 grav = world_gravity_;
		grav *= world_grav_mult;
		physics_world_->SetGravity(grav);

		UpdateBox2d();

		gun_.Update(transform().GetTranslation(), player_gravity_direction_, iam, platform_, camera_, frame_time);

		if (iam->isHeld(MoveLeft) || iam->isHeld(MoveRight)) {
			if (iam->isHeld(MoveLeft)) Rotate(gef::Vector4(0, player_gravity_direction_ != GravityDirection::GRAVITY_UP ? FRAMEWORK_PI : 0, 0));
			else Rotate(gef::Vector4(0, player_gravity_direction_ != GravityDirection::GRAVITY_UP ? 0 : FRAMEWORK_PI, 0));
			if (animation_state_ != JUMPING) animation_state_ = RUNNING;
		}
		else if (iam->isReleased(MoveLeft) || iam->isReleased(MoveRight)) {
			if (animation_state_ != JUMPING) animation_state_ = IDLE;
		}
	}

	anim_time_ += frame_time;

	switch (animation_state_)
	{
	case Player::IDLE:
		set_mesh(sprite_animator3D_->UpdateAnimation(anim_time_, mesh_, "PlayerIdle"));
		break;
	case Player::RUNNING:
		set_mesh(sprite_animator3D_->UpdateAnimation(anim_time_, mesh_, "PlayerRunning"));
		break;
	case Player::JUMPING:
		if(!sprite_animator3D_->ReachedEnd("PlayerJumping")) set_mesh(sprite_animator3D_->UpdateAnimation(anim_time_, mesh_, "PlayerJumping"));
		break;
	case DEATH:
		if (!sprite_animator3D_->ReachedEnd("PlayerDeath")) set_mesh(sprite_animator3D_->UpdateAnimation(anim_time_, mesh_, "PlayerDeath"));
		else {
			level_->SetEndState(EndState::LOSE);
		}
	default:
		break;
	}
}

void Player::BeginCollision(GameObject* other) {
	switch (other->GetTag())
	{
	case Tag::WinObject:
		touching_end_object_ = true;
	case Tag::None:
	case Tag::PressurePlate:
	case Tag::Crate:
	case Tag::Enemy:
		if (jumping_) {
			jumping_ = false;
			animation_state_ = IDLE;
			sprite_animator3D_->Reset("PlayerJumping");
		}
		break;
	case Tag::Bullet: 
	{
     	Bullet* bullet = dynamic_cast<Bullet*>(other);
		if (bullet->getTarget() == GameObject::Tag::Player && bullet->getDamage() > 0) {
			health_--;
			gef::DebugOut("\nhealth: ");
			gef::DebugOut(std::to_string(health_).c_str());
		}
		if (health_ <= 0) {
			animation_state_ = DEATH;
			set_mesh(sprite_animator3D_->GetFirstFrame("PlayerDeath"));
		}
	}
	default:
		break;
	}
}

void Player::EndCollision(GameObject* other) {
	switch (other->GetTag())
	{
	case Tag::WinObject:
		touching_end_object_ = false;
		break;
	default:
		break;
	}
}

void Player::Render(gef::Renderer3D* renderer_3d) {
	renderer_3d->set_override_material(NULL);
	renderer_3d->DrawMesh(*this);
	gun_.Render(renderer_3d);
}

int Player::GetHealth() const
{
	return health_;
}
