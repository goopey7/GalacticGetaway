#include "PressurePlate.h"

#include "graphics/renderer_3d.h"
#include "system/debug_log.h"

void PressurePlate::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, float threshold)
{
 	tag = Tag::PressurePlate;
	threshold_ = threshold;
 
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));
 	physics_world_ = world;
 
 	b2BodyDef body_def;
 	body_def.type = b2_staticBody;
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

void PressurePlate::Update(float frame_time)
{

}

void PressurePlate::BeginCollision(GameObject* other)
{
	GameObject::BeginCollision(other);
}

void PressurePlate::Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const
{
	renderer_3d->set_override_material(&builder->green_material());
	renderer_3d->DrawMesh(*this);
}

void PressurePlate::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, float threshold)
{
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, builder, threshold);
}
