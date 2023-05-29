#pragma once
#include "GameObject.h"
#include <graphics/renderer_3d.h>
#include "PlayerGun.h"
#include "SpriteAnimator3D.h"
#include "Camera.h"

class InputActionManager;
class Level;

class Player : public GameObject {
public:
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, SpriteAnimator3D* sprite_animator, Camera* cam, Level* lev);
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, SpriteAnimator3D* sprite_animator, Camera* cam, Level* lev);
	void Update(InputActionManager* iam, float frame_time);
	bool GetGravityLock() const { return gravity_lock_; }
	bool GetTouchingEnd() { return touching_end_object_; }
	const PlayerGun* GetGun() const { return &gun_; }
	PlayerGun* GetGun() { return &gun_; }
	void BeginCollision(GameObject* other) override;
	void EndCollision(GameObject* other) override;
	void Render(gef::Renderer3D* renderer_3d);

protected:
	Camera* camera_;

	int health_ = 10;
	PlayerGun gun_;

	b2World* physics_world_ = nullptr;
	b2Vec2 world_gravity_ = b2Vec2(0, -1);
	float world_grav_mult = 10;
	bool grav_strength_changed_ = false;
	float grav_strength_change_time = 2;

	bool gravity_lock_ = false;
	bool jumping_ = false;

	GravityDirection world_gravity_direction_ = GravityDirection::GRAVITY_DOWN;
	GravityDirection player_gravity_direction_ = GravityDirection::GRAVITY_DOWN;
	gef::Platform* platform_;

	enum AnimationState { IDLE, RUNNING, JUMPING, DEATH };
	AnimationState animation_state_ = IDLE;

	Level* level_;
	bool touching_end_object_ = false;
};

