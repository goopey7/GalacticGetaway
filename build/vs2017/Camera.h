#pragma once
#include "maths/vector2.h"
#include "maths/vector4.h"
#include "maths/matrix44.h"

class Camera
{
public:
	void Update(float dt, gef::Vector2 target_pos);
	gef::Matrix44 GetViewMatrix() { return view_matrix_; }
	void Warp() { state_ = CameraState::WARP; }
	void Shake() { state_ = CameraState::SHAKE; }
protected:
	gef::Vector2 target_pos_;
	gef::Vector4 camera_pos_;
	gef::Vector4 camera_lookat_;
	gef::Vector4 offset_;
	gef::Vector4 up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix_;
	enum class CameraState { NORMAL, SHAKE, WARP };
	CameraState state_ = CameraState::NORMAL;
	float shake_time_ = 0.2f;
};

