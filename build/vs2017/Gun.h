#pragma once
#include "graphics/sprite.h"
#include "graphics/mesh_instance.h"
#include <system/platform.h>

class InputActionManager;

class Gun : public gef::MeshInstance {
public:
	void Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform, float dt);
	void Fire(float dt);
	void Reload();
	int getAmmoLoaded() { return ammo_loaded_; }
	int getAmmoReserve() { return ammo_reserve_; }
	bool getReloading() { return reloading_; }
protected:
	gef::Vector4 WorldToScreen(const gef::Vector4 pos, gef::Platform* platform);
	void reloadThreadFunc();
	gef::Vector2 target_vector_;
	int ammo_reserve_ = 100;
	int ammo_loaded_ = 30;
	float fire_time_ = 0;
	bool reloading_ = false;
};

