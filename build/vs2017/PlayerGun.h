#pragma once
#include "graphics/mesh_instance.h"
#include <system/platform.h>
#include "BulletManager.h"
#include "Gun.h"

class InputActionManager;

class PlayerGun : public Gun {
public:
	void Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform, float dt);
	void Reload(bool* reloading) override;
	int getAmmoLoaded() const { return ammo_loaded_; }
	int getAmmoReserve() const { return ammo_reserve_; }
	bool getReloading() const { return reloading_; }
	void setReloading(bool r) { reloading_ = r; }
	~PlayerGun();

protected:
	void reloadThreadFunc();
	float GetFireRate() override { return fire_rate_; }
	int* loaded() override { return &ammo_loaded_; }
	void decreaseLoaded() override { ammo_loaded_--; }
	int ammo_reserve_ = 100;
	int ammo_loaded_ = 30;
	int damage_ = 5;
	float fire_rate_ = 1.f / 15.f;
};

