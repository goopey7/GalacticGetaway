#include "Gun.h"
#include "InputActionManager.h"
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <maths/math_utils.h>
#include <cmath>
#include <thread>
#include <system/debug_log.h>

void Gun::Init(gef::Vector4 size, b2World* world, PrimitiveBuilder* builder)
{
	getBulletManager()->Init(world, builder);
}

void Gun::Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform, float dt) {

	//gef::Vector4 screenPos = WorldToScreen(translation, platform);

	transform_.SetTranslation(translation);

	if (input->getUsingKeyboard()) {
		gef::Vector2 mouse_pos = input->getMousePos();
		target_vector_ = mouse_pos - gef::Vector2(platform->width() * 0.5f, platform->height() * 0.5f);
	}
	else {
		gef::Vector2 stick_vec = gef::Vector2(input->getRightStickX(), input->getRightStickY());
		if (!(stick_vec.x == 0 && stick_vec.y == 0)) target_vector_ = stick_vec;
	}
	target_vector_.Normalise();

	gef::Matrix44 rotation_z;
	float angle = atan2(target_vector_.x, target_vector_.y);
	rotation_z.RotationZ(angle);

	gef::Matrix44 translate1;
	translate1.SetIdentity();
	translate1.SetTranslation(gef::Vector4(0, -1.f, (angle >=0) ? 0.1 : -0.1));

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

	if (input->getInputManager()->touch_manager()->is_button_down(0) || input->isHeld(Action::Fire)) {
		Fire(dt);
	}

	if (input->isPressed(Action::Reload)) {
		//reloading_ = true;
		Reload(&reloading_);
		//reloading_ = false;
	}

	bullet_manager_.Update();
}

void Gun::Fire(float dt) {
	if (!reloading_) {
		fire_time_ += dt;
		if (fire_time_ >= 1.f / 15.f && ammo_loaded_ > 0) {
			gef::Vector2 pos(transform().GetTranslation().x(), transform().GetTranslation().y());
			bullet_manager_.Fire(target_vector_, pos, damage_, GameObject::Tag::Enemy, 40.f);
			fire_time_ = 0;
			ammo_loaded_--;
			if (ammo_loaded_ == 0) Reload(&reloading_);
		}
	}
}

void Gun::Reload(bool* reloading) {
	if (!reloading_) {
		reloading_ = true;
		//gef::DebugOut("hey\n");
		std::thread reload_thread([this] { 

			this->reloadThreadFunc(); 
			//gef::DebugOut("hi\n");
			this->setReloading(false);
		});
		reload_thread.detach();
	}
}

void Gun::reloadThreadFunc() {
	//gef::DebugOut(("loaded: " + std::to_string(ammo_loaded_) + " reserve: " + std::to_string(ammo_reserve_) + " delta: " + std::to_string(30 - ammo_loaded_) + "\n").c_str());
	if (ammo_reserve_ > (30 - ammo_loaded_)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ammo_reserve_ -= (30 - ammo_loaded_);
		ammo_loaded_ = 30;
	}
	else if (ammo_reserve_ > 0 && ammo_reserve_ < (30 - ammo_loaded_)) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		ammo_loaded_ += ammo_reserve_;
		ammo_reserve_ = 0;
	}
	else return;
}

void Gun::Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const
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
