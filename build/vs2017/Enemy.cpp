#include "Enemy.h"

void Enemy::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world,
	PrimitiveBuilder* builder, gef::Platform* platform)
{
	tag = Tag::Player;
	platform_ = platform;
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));

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

void Enemy::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder,
	gef::Platform* platform)
{
	platform_ = platform;
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
	physics_body_->SetFixedRotation(true);

	UpdateBox2d();
}

void Enemy::Update(float frame_time)
{
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
	
	// TODO Raycast to check if player is in sight
		// TODO Shoot at player if in sight
	// TODO else
		// TODO Movement depending on gravity. Maybe move like red Koopas?
		switch (world_gravity_direction_)
		{
		case GRAVITY_VERTICAL:
			//...
			break;
		case GRAVITY_LEFT:
			//...
			break;
		case GRAVITY_RIGHT:
			//...
			break;
		}

	UpdateBox2d();
}

void Enemy::BeginCollision(GameObject* other)
{
}

void Enemy::Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const
{
	renderer_3d->set_override_material(&builder->red_material());
	renderer_3d->DrawMesh(*this);
}
