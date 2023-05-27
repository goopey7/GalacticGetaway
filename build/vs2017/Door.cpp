#include "Door.h"
#include "graphics/renderer_3d.h"

Door::Door(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, gef::Mesh* door_wall, gef::Mesh* door_frame, gef::Mesh* door) {

	gef::Matrix44 transform_matrix;
	transform_matrix.SetIdentity();
	transform_matrix.SetTranslation(pos);
	door_wall_.set_transform(transform_matrix);
	door_frame_.set_transform(transform_matrix);

	door_wall_.set_mesh(door_wall);
	door_frame_.set_mesh(door_frame);

	door_ = new GameObject();
	door_->Init(size, pos, world, builder);
	door_->set_mesh(door);

	closed_pos_ = pos;
}

void Door::Update(float dt) {

}

void Door::Open() {

}

void Door::Render(gef::Renderer3D* renderer_3d) const {
	door_->Render(renderer_3d);
	renderer_3d->DrawMesh(door_frame_);
	renderer_3d->DrawMesh(door_wall_);
}