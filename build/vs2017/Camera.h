#pragma once
#include "maths/vector2.h"
#include "maths/vector4.h"
#include "maths/matrix44.h"
#include <queue>

class Camera
{
public:
	void Update(float dt, gef::Vector2 target_pos);
	gef::Matrix44 GetViewMatrix() { return view_matrix_; }
	void Warp() { effect_state_ = EffectState::WARP; }
	void Shake();
	void SetPosition(gef::Vector4 pos);
protected:
	gef::Vector4 target_pos_;
	gef::Vector4 camera_pos_;
	gef::Vector4 camera_lookat_;
	gef::Vector4 lerp_start_;
	gef::Vector4 offset_;
	gef::Vector4 up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix_;
	enum class EffectState { NORMAL, SHAKE, WARP };
	EffectState effect_state_ = EffectState::NORMAL;
	float shake_time_ = 0.2f;
	enum class MoveState { STATIONARY, LERP, TRACK };
	MoveState move_state_ = MoveState::STATIONARY;
	float lerp_time_ = 0.2f;
	std::queue<gef::Vector4> prev_lookats_;
	gef::Vector4 shake_start_lookat_;
};

