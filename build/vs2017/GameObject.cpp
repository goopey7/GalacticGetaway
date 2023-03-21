#include "GameObject.h"
#include "primitive_builder.h"

void GameObject::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, bool dynamic) {
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x/2.f, size_y/2.f, size_z/2.f)));

	b2BodyDef body_def;
	dynamic ? body_def.type = b2_dynamicBody : body_def.type = b2_staticBody;
	body_def.position = b2Vec2(pos_x, pos_y);

	b2PolygonShape shape;
	shape.SetAsBox(size_x/2.f, size_y/2.f);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.7f;

	physics_body_ = world->CreateBody(&body_def);
	physics_body_->CreateFixture(&fixture);
	physics_body_->GetUserData().pointer = (uintptr_t)this;

	UpdateBox2d();
}

void GameObject::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, bool dynamic) {
	set_mesh(builder->CreateBoxMesh(size/2.f));

	b2BodyDef body_def;
	dynamic ? body_def.type = b2_dynamicBody : body_def.type = b2_staticBody;
	body_def.position = b2Vec2(pos.x(), pos.y());

	b2PolygonShape shape;
	shape.SetAsBox(size.x()/2.f, size.y()/2.f);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.f;
	fixture.friction = 0.5f;

	physics_body_ = world->CreateBody(&body_def);
	physics_body_->CreateFixture(&fixture);
	physics_body_->GetUserData().pointer = (uintptr_t)this;

	UpdateBox2d();
}

void GameObject::Update() {
	UpdateBox2d();
}

void GameObject::UpdateBox2d() {
	gef::Matrix44 transform;
	transform.SetIdentity();

	gef::Matrix44 rotation_z;
	rotation_z.RotationZ(physics_body_->GetAngle());

	gef::Matrix44 translation;
	translation.SetIdentity();
	translation.SetTranslation(gef::Vector4(physics_body_->GetPosition().x, physics_body_->GetPosition().y, 0.f));

	transform = rotation_z * translation;
	set_transform(transform);
}