#include "Gun.h"
#include "InputActionManager.h"
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <maths/math_utils.h>
#include "SpriteAnimator3D.h"
#include <cmath>
#include <thread>
#include <system/debug_log.h>

void Gun::Init(gef::Vector4 size, b2World* world, SpriteAnimator3D* sprite_animator, const char* filename)
{
	set_mesh(sprite_animator->CreateMesh(filename, size));
	getBulletManager()->Init(world, sprite_animator->GetPrimitiveBuilder());
}

void Gun::Update(gef::Vector4 translation) {
	ammo_reserve_ = INT_MAX;
	ammo_loaded_ = INT_MAX;

	UpdateTransform(translation);

	bullet_manager_.Update();
}

void Gun::UpdateTransform(gef::Vector4 translation) {
	transform_.SetTranslation(translation);

	gef::Matrix44 rotation_z;
	float angle = atan2(target_vector_.x, target_vector_.y);
	rotation_z.RotationZ(angle);

	gef::Matrix44 translate1;
	translate1.SetIdentity();
	translate1.SetTranslation(gef::Vector4(0, -1.f, (angle >= 0) ? 0.1 : -0.1));

	gef::Matrix44 translate2;
	translate2.SetIdentity();
	translate2.SetTranslation(translation);

	if (angle < 0) {
		gef::Matrix44 rotation_y;
		rotation_y.RotationY(FRAMEWORK_PI);
		translate1 = translate1 * rotation_y;
	}
	translate1 = translate1 * rotation_z * translate2;

	set_transform(translate1);
}

void Gun::Fire(float dt, GameObject::Tag target) {
	if (!reloading_) {
		fire_time_ += dt;
		if (fire_time_ >= GetFireRate() && ammo_loaded_ > 0) {
			gef::Vector2 pos(transform().GetTranslation().x(), transform().GetTranslation().y());
			bullet_manager_.Fire(target_vector_, pos, damage_, target, 40.f);
			fire_time_ = 0;
			ammo_loaded_--;
			if (ammo_loaded_ == 0) Reload(&reloading_);
		}
	}
}

void Gun::Render(gef::Renderer3D* renderer_3d) const
{
	renderer_3d->DrawMesh(*this);
	getBulletManager()->Render(renderer_3d);
}

Gun::~Gun()
{
}

gef::Vector4 Gun::WorldToScreen(const gef::Vector4 pos, gef::Platform* platform) {
	gef::Vector4 screen_pos = pos;
	gef::DebugOut(("\npos: " + std::to_string(pos.x()) + ", " + std::to_string(pos.y())).c_str());
	screen_pos *= 33;
	screen_pos += gef::Vector4(platform->width() * 0.5f, -platform->height() * 0.5f, 0);
	screen_pos.set_y(-screen_pos.y());
	return screen_pos;
}
