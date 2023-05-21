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
};

