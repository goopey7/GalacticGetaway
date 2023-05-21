#pragma once
#include "GameObject.h"
#include <graphics/renderer_3d.h>
#include "PlayerGun.h"
#include "SpriteAnimator3D.h"

class InputActionManager;

class Player : public GameObject {
public:
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, SpriteAnimator3D* sprite_animator);
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, SpriteAnimator3D* sprite_animator);
	void Update(InputActionManager* iam, float frame_time);
	bool GetGravityLock() const { return gravity_lock_; }
	const PlayerGun* GetGun() const { return &gun_; }
	void BeginCollision(GameObject* other) override;
	void Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder);

protected:
	int health_ = 10;
	PlayerGun gun_;

	b2World* physics_world_;
	b2Vec2 world_gravity_ = b2Vec2(0, -1);
	float world_grav_mult = 10;
	bool grav_strength_changed_ = false;
	float grav_strength_change_time = 2;

	bool gravity_lock_ = false;
	bool jumping_ = false;
	enum GravityDirection { GRAVITY_VERTICAL, GRAVITY_LEFT, GRAVITY_RIGHT };
	GravityDirection world_gravity_direction_ = GRAVITY_VERTICAL;
	GravityDirection player_gravity_direction_ = GRAVITY_VERTICAL;
	gef::Platform* platform_;

	enum AnimationState { IDLE, RUNNING, JUMPING };
	AnimationState animation_state_ = IDLE;
};

