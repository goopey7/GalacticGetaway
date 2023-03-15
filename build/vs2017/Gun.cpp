#include "Gun.h"
#include "InputActionManager.h"
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <maths/math_utils.h>
#include <cmath>
#include <thread>
#include <system/debug_log.h>

void Gun::Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform, float dt) {

	gef::Vector4 screenPos = WorldToScreen(translation, platform);

	transform_.SetTranslation(translation);

	if (input->getUsingKeyboard()) {
		gef::Vector2 mouse_pos = input->getMousePos();
		target_vector_ = mouse_pos - gef::Vector2(screenPos.x(), screenPos.y());
	}
	else {
		gef::Vector2 stick_vec = gef::Vector2(input->getRightStickX(), input->getRightStickY());
		if (!(stick_vec.x == 0 && stick_vec.y == 0)) target_vector_ = stick_vec;
	}
	target_vector_.Normalise();

	float angle = atan2(target_vector_.x, target_vector_.y);
	if (angle < 0) angle += 2 * FRAMEWORK_PI;

	gef::Matrix44 rotation_z;
	rotation_z.RotationZ(angle);

	gef::Matrix44 translate1;
	translate1.SetIdentity();
	translate1.SetTranslation(gef::Vector4(0, -0.5f, 0));

	gef::Matrix44 translate2;
	translate2.SetIdentity();
	translate2.SetTranslation(translation);

	rotation_z = translate1 * rotation_z * translate2;
	set_transform(rotation_z);

	if (input->getInputManager()->touch_manager()->is_button_down(0) || input->isHeld(Action::Fire)) {
		Fire(dt);
	}
	if (input->isPressed(Action::Reload)) {
		Reload();
	}
}

void Gun::Fire(float dt) {
	fire_time_ += dt;
	if (fire_time_ >= 1.f / 15.f && ammo_loaded_ > 0) {
		fire_time_ = 0;
		ammo_loaded_--;
		if (ammo_loaded_ == 0) Reload();
	}
}

void Gun::Reload() {
	std::thread reload_thread([this] { this->reloadThreadFunc(); });
	reload_thread.detach();
}

void Gun::reloadThreadFunc() {
	if (ammo_reserve_ > (30 - ammo_loaded_)) {
		reloading_ = true;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		reloading_ = false;
		ammo_reserve_ -= (30 - ammo_loaded_);
		ammo_loaded_ = 30;
	}
	else if (ammo_reserve_ > 0 && ammo_reserve_ < (30 - ammo_loaded_)) {
		reloading_ = true;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		reloading_ = false;
		ammo_loaded_ += ammo_reserve_;
		ammo_reserve_ = 0;
	}
	else return;
}

gef::Vector4 Gun::WorldToScreen(const gef::Vector4 pos, gef::Platform* platform) {
	gef::Vector4 screen_pos = pos;
	screen_pos *= 33;
	screen_pos += gef::Vector4(platform->width() * 0.5f, -platform->height() * 0.5f, 0);
	screen_pos.set_y(-screen_pos.y());
	return screen_pos;
}