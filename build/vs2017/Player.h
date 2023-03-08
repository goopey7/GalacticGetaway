#pragma once
#include "GameObject.h"
#include "input/input_manager.h"

class Player : public GameObject {
public:
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder);
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder);
	void Update(gef::InputManager* input, float frame_time);
	bool GetGravityLock() { return gravity_lock_; }

protected:
	int health_ = 10;

	b2World* physics_world_;
	bool gravity_lock_ = false;
	enum GravityDirection { GRAVITY_VERTICAL, GRAVITY_LEFT, GRAVITY_RIGHT };
	GravityDirection world_gravity_direction_ = GRAVITY_VERTICAL;
	GravityDirection player_gravity_direction_ = GRAVITY_VERTICAL;
};

