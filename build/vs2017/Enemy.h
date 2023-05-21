﻿#pragma once

#include "GameObject.h"
#include <graphics/renderer_3d.h>

#include "BulletManager.h"

class Player;

class Enemy : public GameObject, public b2RayCastCallback
{
public:
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, SpriteAnimator3D* sprite_animator, const ::Player* player);
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, SpriteAnimator3D* sprite_animator, const ::Player* player);
	void Update(float frame_time);

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

	void BeginCollision(GameObject* other) override;

	void Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const;
	
protected:
	int health_ = 10;
	int damage_ = 1;
	float fire_rate_ = 1.f; // shots per second
	float move_speed_ = 4.f;
	bool moving_left_ = true;

	b2World* physics_world_;
	b2Vec2 world_gravity_ = b2Vec2(0, -1);
	float world_grav_mult = 10;
	bool grav_strength_changed_ = false;
	float grav_strength_change_time = 2;

	bool gravity_lock_ = false;
	bool jumping_ = false;
	enum GravityDirection { GRAVITY_VERTICAL, GRAVITY_LEFT, GRAVITY_RIGHT };
	GravityDirection world_gravity_direction_ = GRAVITY_VERTICAL;
	const gef::Platform* platform_;

	const ::Player* player_ = nullptr;
	bool bPlayerInRange_ = false;
	bool bSawPlayer = false;
	float player_detection_range_ = 50.f;

	BulletManager bullet_manager_;
	float fire_timer_ = 0.f;

	float size_y_ = 0.f;

	enum AnimationState { IDLE, RUNNING };
	AnimationState animation_state_ = RUNNING;
};
