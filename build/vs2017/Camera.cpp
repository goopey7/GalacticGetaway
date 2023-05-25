#include "Camera.h"
#include "maths/math_utils.h"
#include <cmath>

void Camera::Update(float dt, gef::Vector2 target_pos) {
	if (state_ == CameraState::NORMAL) {
		offset_ = gef::Vector4(0, 0, 0);
		camera_pos_ = gef::Vector4(target_pos.x, target_pos.y + 3, 30);
		camera_lookat_ = gef::Vector4(target_pos.x, target_pos.y, 0);
	}
	else {
		if (shake_time_ <= 0.0f) {
			state_ = CameraState::NORMAL;
			shake_time_ = 0.2f;
		}
		shake_time_ -= dt;

		if (state_ == CameraState::SHAKE) {
			offset_ = gef::Vector4(0.1 * sin(40 * shake_time_), 0.1 * sin(15 * shake_time_), 0);
			camera_pos_ = gef::Vector4(target_pos.x, target_pos.y + 3, 30);
			camera_lookat_ = gef::Vector4(target_pos.x, target_pos.y, 0) + offset_;
		}
		else if (state_ == CameraState::WARP) {
			offset_ = gef::Vector4(sin(40 * shake_time_), sin(15 * shake_time_), 0);
			camera_pos_ = gef::Vector4(target_pos.x, target_pos.y + 3, 30) + offset_;
			camera_lookat_ = gef::Vector4(target_pos.x, target_pos.y, 0);
		}
	}
	view_matrix_.SetIdentity();
	view_matrix_.LookAt(camera_pos_, camera_lookat_, up_);
}