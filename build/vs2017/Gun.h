#pragma once
#include "graphics/mesh_instance.h"
#include <system/platform.h>
#include "BulletManager.h"

class InputActionManager;

class Gun : public gef::MeshInstance {
public:
	void Init(gef::Vector4 size, b2World* world, PrimitiveBuilder* builder);
	void Update(gef::Vector4 translation, InputActionManager* input, gef::Platform* platform, float dt);
	void Fire(float dt);
	void Reload(bool* reloading);
	int getAmmoLoaded() const { return ammo_loaded_; }
	int getAmmoReserve() const { return ammo_reserve_; }
	bool getReloading() const { return reloading_; }
	void setReloading(bool r) { reloading_ = r; }
	const BulletManager* getBulletManager() const { return &bullet_manager_; }
	BulletManager* getBulletManager() { return &bullet_manager_; }
	void Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const;
	~Gun();

protected:
	gef::Vector4 WorldToScreen(const gef::Vector4 pos, gef::Platform* platform);
	void reloadThreadFunc();
	gef::Vector2 target_vector_;
	int ammo_reserve_ = 100;
	int ammo_loaded_ = 30;
	float fire_time_ = 0;
	int damage_ = 5;
	bool reloading_ = false;

	BulletManager bullet_manager_;
};

