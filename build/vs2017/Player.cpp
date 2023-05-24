#include "Player.h"
#include <input/keyboard.h>
#include <maths/vector2.h>
#include <maths/math_utils.h>
#include "system/debug_log.h"

#include "InputActionManager.h"

void Player::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, SpriteAnimator3D* sprite_animator) {
	tag = Tag::Player;
	platform_ = sprite_animator->GetPlatform();
	sprite_animator3D_ = sprite_animator;
	set_mesh(sprite_animator3D_->GetFirstFrame("PlayerIdle"));

	gun_.Init(gef::Vector4(size_x * 0.33f, size_y, size_z), world, sprite_animator, "Player/Gun/gun.png");

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

void Player::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, SpriteAnimator3D* sprite_animator) {
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, sprite_animator);
}

void Player::Update(InputActionManager* iam, float frame_time) {
	// Movement
	if (iam->isHeld(MoveLeft)) {
		switch (player_gravity_direction_)
		{
		case GRAVITY_VERTICAL:
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(-8 * frame_time, 0), physics_body_->GetAngle());
			break;
		case GRAVITY_LEFT:
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, 8 * frame_time), physics_body_->GetAngle());
			break;
		case GRAVITY_RIGHT:
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, -8 * frame_time), physics_body_->GetAngle());
			break;
		default:
			break;
		}
	}
	if (iam->isHeld(MoveRight)) {
		switch (player_gravity_direction_)
		{
		case GRAVITY_VERTICAL:
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(8 * frame_time, 0), physics_body_->GetAngle());
			break;
		case GRAVITY_LEFT:
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, -8 * frame_time), physics_body_->GetAngle());
			break;
		case GRAVITY_RIGHT:
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
		world_gravity_ = b2Vec2(0, 1);
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(180));
		world_gravity_direction_ = GRAVITY_VERTICAL;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_VERTICAL;
	}
	else if (iam->isPressed(GravityDown)) {
		world_gravity_ = b2Vec2(0, -1);
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), 0);
		world_gravity_direction_ = GRAVITY_VERTICAL;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_VERTICAL;
	}
	else if (iam->isPressed(GravityLeft)) {
		world_gravity_ = b2Vec2(-1, 0);
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(-90));
		world_gravity_direction_ = GRAVITY_LEFT;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_LEFT;
	}
	else if (iam->isPressed(GravityRight)) {
		world_gravity_ = b2Vec2(1, 0);
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(90));
		world_gravity_direction_ = GRAVITY_RIGHT;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_RIGHT;
	}

	if (iam->isPressed(GravityStrenghtUp)) {
		grav_strength_changed_ = true;
		world_grav_mult = 50.f;
	}
	else if (iam->isPressed(GravityStrengthDown)) {
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

	gun_.Update(transform().GetTranslation(), iam, platform_, frame_time);

	anim_time_ += frame_time;
	if (iam->isHeld(MoveLeft) || iam->isHeld(MoveRight)) {
		if (iam->isHeld(MoveLeft)) Rotate(gef::Vector4(0, FRAMEWORK_PI, 0));
		else Rotate(gef::Vector4(0, 0, 0));
		if (animation_state_ != JUMPING) animation_state_ = RUNNING;
	}
	else if (iam->isReleased(MoveLeft) || iam->isReleased(MoveRight)) {
		if (animation_state_ != JUMPING) animation_state_ = IDLE;
	}
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
	default:
		break;
	}
}

void Player::BeginCollision(GameObject* other) {
	switch (other->GetTag())
	{
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
	default:
		break;
	}
}

void Player::Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) {
	renderer_3d->set_override_material(NULL);
	renderer_3d->DrawMesh(*this);
	gun_.Render(renderer_3d);
}