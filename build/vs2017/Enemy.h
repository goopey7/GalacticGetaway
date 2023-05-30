#pragma once

#include "GameObject.h"
#include <graphics/renderer_3d.h>
#include "Gun.h"
#include "Pickup.h"

class Player;

class Enemy : public GameObject, public b2RayCastCallback
{
public:
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, SpriteAnimator3D
			* sprite_animator, gef::AudioManager* am, const Player* player, std::vector<GameObject*>& dynamic_game_objects);
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, SpriteAnimator3D* sprite_animator, gef
			::AudioManager* am, const Player* player, std::vector<GameObject*>& dynamic_game_objects);
	void Update(float frame_time);

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
	void InspectClosestFixture();

	void BeginCollision(GameObject* other) override;

	void Render(gef::Renderer3D* renderer_3d) const;
	
protected:
	int health_ = 10;
	int damage_ = 1;
	float fire_rate_ = 1.f; // shots per second
	float move_speed_ = 4.f;
	bool moving_left_ = true;

	b2World* physics_world_ = nullptr;
	b2Vec2 world_gravity_ = b2Vec2(0, -1);

	GravityDirection world_gravity_direction_ = GravityDirection::GRAVITY_DOWN;
	const gef::Platform* platform_;

	const ::Player* player_ = nullptr;
	bool bPlayerInRange_ = false;
	float player_detection_range_ = 10.f;
	float closest_fraction_ = 1.0f;
	b2Fixture* closest_fixture_ = nullptr;

	Gun gun_;

	Pickup* pickup_ = nullptr;

	enum AnimationState { IDLE, RUNNING, DEATH };
	AnimationState animation_state_ = RUNNING;

	std::vector<GameObject*>* dynamic_game_objects_ = nullptr;
	PrimitiveBuilder* primitive_builder_ = nullptr;

	float drop_probability_ = 0.5f;

	SpriteAnimator3D* sprite_animator_ = nullptr;
};
