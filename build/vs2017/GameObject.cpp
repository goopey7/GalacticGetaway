#include "GameObject.h"
#include "primitive_builder.h"
#include "graphics/renderer_3d.h"

void GameObject::Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, bool dynamic) {

	size_ = gef::Vector4(size_x, size_y, size_z);
	set_mesh(builder->CreateBoxMesh(gef::Vector4(size_x, size_y, size_z)));

	b2BodyDef body_def;
	dynamic ? body_def.type = b2_dynamicBody : body_def.type = b2_staticBody;
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

	UpdateBox2d();
}

void GameObject::Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, bool dynamic) {
	Init(size.x(), size.y(), size.z(), pos.x(), pos.y(), world, builder, dynamic);
}

void GameObject::Update(float frame_time) {
	UpdateBox2d();
}

void GameObject::Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const
{
	renderer_3d->DrawMesh(*this);
}

void GameObject::BeginCollision(GameObject* other)
{
}

void GameObject::EndCollision(GameObject* other)
{
}

void GameObject::PreResolve(GameObject* other)
{
}

void GameObject::PostResolve(GameObject* other)
{
}

void GameObject::EnableCollisionResolution(bool enable)
{
	b2Fixture* fixture = physics_body_->GetFixtureList();
	while(fixture != nullptr)
	{
		fixture->SetSensor(!enable);
		fixture = fixture->GetNext();
	}
}

GameObject::Tag GameObject::GetTag()
{
	return tag;
}

void GameObject::SetTag(Tag tag)
{
	this->tag = tag;
}

void GameObject::Kill()
{
	dead = true;
}

bool GameObject::TimeToDie()
{
	return dead;
}

void GameObject::UpdateBox2d() {
	gef::Matrix44 transform;
	transform.SetIdentity();

	gef::Matrix44 rotation;
	rotation.RotationX(rotate_.x());
	rotation.RotationZ(rotate_.z() + physics_body_->GetAngle());
	rotation.RotationY(rotate_.y());

	gef::Matrix44 translation1;
	translation1.SetIdentity();
	if (!(translate_.x() == 0 && translate_.y() == 0 && translate_.z() == 0)) {
		translation1.SetTranslation(translate_);
	}

	gef::Matrix44 translation2;
	translation2.SetIdentity();
	translation2.SetTranslation(gef::Vector4(physics_body_->GetPosition().x, physics_body_->GetPosition().y, 0.f));

	transform = rotation * translation1 * translation2;
	set_transform(transform);
}