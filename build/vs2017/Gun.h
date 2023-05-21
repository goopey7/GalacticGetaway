#pragma once
#include "graphics/mesh_instance.h"
#include <system/platform.h>
#include "BulletManager.h"

class InputActionManager;

class Gun : public gef::MeshInstance {
public:
	void Init(gef::Vector4 size, b2World* world, SpriteAnimator3D* sprite_animator, const char* filename);
	void Update(gef::Vector4 translation);
	void Fire(float dt);
	virtual void Reload(bool* reloading) {};
	const BulletManager* getBulletManager() const { return &bullet_manager_; }
	BulletManager* getBulletManager() { return &bullet_manager_; }
	void Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const;
	~Gun();

protected:
	void UpdateTransform(gef::Vector4 translation);
	gef::Vector4 WorldToScreen(const gef::Vector4 pos, gef::Platform* platform);
	gef::Vector2 target_vector_;
	int ammo_reserve_ = 100;
	int ammo_loaded_ = 30;
	int damage_ = 1;
	float fire_time_ = 0;
	bool reloading_ = false;

	BulletManager bullet_manager_;
};

