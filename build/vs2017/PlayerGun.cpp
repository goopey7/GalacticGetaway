#include "PlayerGun.h"
#include "InputActionManager.h"
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <maths/math_utils.h>
#include "SpriteAnimator3D.h"
#include <cmath>
#include <thread>
#include <system/debug_log.h>

void PlayerGun::Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform, float dt) {

	if (input->getUsingKeyboard()) {
		gef::Vector2 mouse_pos = input->getMousePos();
		target_vector_ = mouse_pos - gef::Vector2(platform->width() * 0.5f, platform->height() * 0.5f);
	}
	else {
		gef::Vector2 stick_vec = gef::Vector2(input->getRightStickX(), input->getRightStickY());
		if (!(stick_vec.x == 0 && stick_vec.y == 0)) target_vector_ = stick_vec;
	}
	target_vector_.Normalise();

	UpdateTransform(translation);

	if (input->getInputManager()->touch_manager()->is_button_down(0) || input->isHeld(Action::Fire)) {
		Fire(dt, GameObject::Tag::Enemy);
	}

	if (input->isPressed(Action::Reload)) {
		Reload(&reloading_);
	}

	bullet_manager_.Update();
}

void PlayerGun::Reload(bool* reloading) {
	if (!reloading_) {
		reloading_ = true;
		std::thread reload_thread([this] { 

			this->reloadThreadFunc(); 
			this->setReloading(false);
		});
		reload_thread.detach();
	}
}

void PlayerGun::reloadThreadFunc() {
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

PlayerGun::~PlayerGun()
{
}