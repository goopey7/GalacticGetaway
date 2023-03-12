#pragma once
#include "GameObject.h"
#include "input/input_manager.h"
#include <graphics/renderer_3d.h>
#include "Gun.h"

class InputActionManager;

class Player : public GameObject {
public:
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder);
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder);
	void Update(InputActionManager* iam, float frame_time);
	bool GetGravityLock() { return gravity_lock_; }

	void Render(gef::Renderer3D* renderer, PrimitiveBuilder* builder);

protected:
	int health_ = 10;
	Gun gun_;

	b2World* physics_world_;
	bool gravity_lock_ = false;
	enum GravityDirection { GRAVITY_VERTICAL, GRAVITY_LEFT, GRAVITY_RIGHT };
	GravityDirection world_gravity_direction_ = GRAVITY_VERTICAL;
	GravityDirection player_gravity_direction_ = GRAVITY_VERTICAL;
};

