#include "Player.h"
#include <input/keyboard.h>
#include <maths/vector2.h>
#include <maths/math_utils.h>

#include "InputActionManager.h"

void Player::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder) {
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));

	physics_world_ = world;

	b2BodyDef body_def;
	body_def.type = b2_dynamicBody;
	body_def.position = b2Vec2(pos_x, pos_y);

	b2PolygonShape shape;
	shape.SetAsBox(size_x, size_y);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.7f;

	physics_body_ = world->CreateBody(&body_def);
	physics_body_->CreateFixture(&fixture);
	physics_body_->GetUserData().pointer = (uintptr_t)this;
	physics_body_->SetSleepingAllowed(false);

	UpdateBox2d();
}

void Player::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder) {
	set_mesh(builder->CreateBoxMesh(size));

	physics_world_ = world;

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

	UpdateBox2d();
}


void Player::Update(InputActionManager* iam, float frame_time) {
	
	// Movement
	switch (player_gravity_direction_)
	{
	case GRAVITY_VERTICAL:
		if(iam->isHeld(MoveLeft)) {
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(-8 * frame_time, 0), 0);
		}
		if(iam->isHeld(MoveRight)) {
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(8 * frame_time, 0), 0);
		}
		break;
	case GRAVITY_LEFT:
		if(iam->isHeld(MoveLeft)) {
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, 8 * frame_time), 0);
		}
		if(iam->isHeld(MoveRight)) {
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, -8 * frame_time), 0);
		}
		break;
	case GRAVITY_RIGHT:
		if(iam->isHeld(MoveLeft)) {
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, -8 * frame_time), 0);
		}
		if(iam->isHeld(MoveRight)) {
			physics_body_->SetTransform(physics_body_->GetPosition() + b2Vec2(0, 8 * frame_time), 0);
		}
		break;
	default:
		break;
	}

	if (iam->isPressed(GravityLock)) {
		gravity_lock_ = !gravity_lock_;
		if (gravity_lock_) physics_body_->SetGravityScale(0);
		else {
			physics_body_->SetGravityScale(1);
			b2Vec2 grav = physics_world_->GetGravity();
			grav *= 0.01;
			physics_body_->ApplyLinearImpulseToCenter(grav, true);
			player_gravity_direction_ = world_gravity_direction_;
		}
	}

	if (iam->isPressed(Jump) && !gravity_lock_) {
		b2Vec2 grav = physics_world_->GetGravity();
		grav *= 0.6;
		physics_body_->ApplyLinearImpulseToCenter(-grav, true);
	}

	if (iam->isPressed(GravityUp)) {
		physics_world_->SetGravity(b2Vec2(0, 9.8f));
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(180));
		world_gravity_direction_ = GRAVITY_VERTICAL;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_VERTICAL;
	}
	else if (iam->isPressed(GravityDown)) {
		physics_world_->SetGravity(b2Vec2(0, -9.8f));
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), 0);
		world_gravity_direction_ = GRAVITY_VERTICAL;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_VERTICAL;
	}
	else if (iam->isPressed(GravityLeft)) {
		physics_world_->SetGravity(b2Vec2(-9.8f, 0));
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(90));
		world_gravity_direction_ = GRAVITY_LEFT;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_LEFT;
	}
	else if (iam->isPressed(GravityRight)) {
		physics_world_->SetGravity(b2Vec2(9.8f, 0));
		physics_world_->SetAllowSleeping(false);
		physics_body_->SetTransform(physics_body_->GetPosition(), gef::DegToRad(-90));
		world_gravity_direction_ = GRAVITY_RIGHT;
		if (!gravity_lock_) player_gravity_direction_ = GRAVITY_RIGHT;
	}

	UpdateBox2d();
}