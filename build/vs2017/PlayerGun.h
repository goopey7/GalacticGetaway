#pragma once
#include "graphics/mesh_instance.h"
#include <system/platform.h>
#include "BulletManager.h"
#include "Gun.h"
#include "Camera.h"

class InputActionManager;

class PlayerGun : public Gun {
public:
	void Update(gef::Vector4 translation, GravityDirection grav_dir, InputActionManager* input, gef::Platform* platform, Camera* cam, float dt);
	void Reload(bool* reloading) override;
	int getAmmoLoaded() const { return ammo_loaded_; }
	int getAmmoReserve() const { return ammo_reserve_; }
	bool getReloading() const { return reloading_; }
	void setReloading(bool r) { reloading_ = r; }
	~PlayerGun();
	void maxAmmo();

protected:
	void reloadThreadFunc();
	float GetFireRate() override { return fire_rate_; }
	int* loaded() override { return &ammo_loaded_; }
	void decreaseLoaded() override { ammo_loaded_--; }
	const int max_ammo_loaded_ = 30;
	const int max_ammo_reserve_ = 100;
	
	int ammo_reserve_ = max_ammo_reserve_;
	int ammo_loaded_ = max_ammo_loaded_;
	int damage_ = 5;
	float fire_rate_ = 1.f / 15.f;
};

