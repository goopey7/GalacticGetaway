#include "Door.h"
#include "graphics/renderer_3d.h"

Door::Door(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, gef::AudioManager* am, gef::Mesh* door_wall, gef::Mesh* door_frame, gef::Mesh* door) {

	gef::Matrix44 transform_matrix;
	transform_matrix.SetIdentity();
	transform_matrix.SetTranslation(pos);
	door_wall_.set_transform(transform_matrix);
	door_frame_.set_transform(transform_matrix);

	door_wall_.set_mesh(door_wall);
	door_frame_.set_mesh(door_frame);

	door_ = new GameObject();
	door_->Init(size, pos, world, builder, am);
	door_->set_mesh(door);

	closed_pos_ = pos;
	open_pos_ = closed_pos_ - gef::Vector4(0, door_->GetSize().y() * 1.4f, 0);
}

void Door::Update(float dt) {
	gef::Vector4 translation;
	switch (current_state_)
	{
	case Door::State::OPENING:
		lerp_time_ += dt;
		translation;
		translation.Lerp(start_pos_, open_pos_, lerp_time_);
		if ((open_pos_ - translation).LengthSqr() <= 0.01) {
			translation = open_pos_;
			current_state_ = State::IDLE;
			door_->GetBody()->SetEnabled(false);
		}
		door_->GetBody()->SetTransform(b2Vec2(translation.x(), translation.y()), door_->GetBody()->GetAngle());
		door_->UpdateBox2d();
		break;
	case Door::State::CLOSING:
		door_->GetBody()->SetEnabled(true);
		lerp_time_ += dt;
		translation;
		translation.Lerp(start_pos_, closed_pos_, lerp_time_);
		if ((closed_pos_ - translation).LengthSqr() <= 0.01) {
			translation = closed_pos_;
			current_state_ = State::IDLE;
		}
		door_->GetBody()->SetTransform(b2Vec2(translation.x(), translation.y()), door_->GetBody()->GetAngle());
		door_->UpdateBox2d();
		break;
	case Door::State::IDLE:
		break;
	default:
		break;
	}

}

void Door::Open() {
	start_pos_ = door_->transform().GetTranslation();
	lerp_time_ = 0.f;
	current_state_ = State::OPENING;
}

void Door::Close() {
	start_pos_ = door_->transform().GetTranslation();
	lerp_time_ = 0.f;
	current_state_ = State::CLOSING;
}

void Door::Render(gef::Renderer3D* renderer_3d) const {
	door_->Render(renderer_3d);
	renderer_3d->DrawMesh(door_frame_);
	renderer_3d->DrawMesh(door_wall_);
}