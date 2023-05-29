#pragma once
#include "graphics/mesh_instance.h"
#include <system/platform.h>
#include "BulletManager.h"

class InputActionManager;
enum class GravityDirection;

class Gun : public gef::MeshInstance {
public:
	void Init(gef::Vector4 size, b2World* world, SpriteAnimator3D* sprite_animator, gef::AudioManager* am, const char* filename);
	void Update(float frame_time, gef::Vector4 translation, GravityDirection grav_dir);
	void Fire(float dt, GameObject::Tag target);
	virtual void Reload(bool* reloading) {};
	void SetTargetVector(gef::Vector2 vec) { target_vector_ = vec; target_vector_.Normalise(); }
	void SetDamage(int dam) { damage_ = dam; }
	void SetFireRate(int rate) { fire_rate_ = rate; }
	const BulletManager* getBulletManager() const { return &bullet_manager_; }
	BulletManager* getBulletManager() { return &bullet_manager_; }
	void Render(gef::Renderer3D* renderer_3d) const;
	virtual int* loaded() { return &ammo_loaded_; }
	~Gun();

protected:

	virtual void decreaseLoaded() { ammo_loaded_--; }
	virtual float GetFireRate() { return fire_rate_; }
	void UpdateTransform(gef::Vector4 translation, GravityDirection grav_dir);
	gef::Vector4 WorldToScreen(const gef::Vector4 pos, gef::Platform* platform);
	gef::Vector2 target_vector_;

	float fire_time_ = 0;
	bool reloading_ = false;

	BulletManager bullet_manager_;
	gef::AudioManager* am_;
private:

	int ammo_reserve_ = INT_MAX;
	int ammo_loaded_ = INT_MAX;
	int damage_ = 1;
	float fire_rate_ = 1;
};

