#include "Camera.h"
#include "maths/math_utils.h"
#include <cmath>
#include "system/debug_log.h"
#include <string>

void Camera::Update(float dt, gef::Vector2 target_pos) {
	target_pos_ = gef::Vector4(target_pos.x, target_pos.y + 3, 0);

	switch (move_state_)
	{
	case Camera::MoveState::STATIONARY:
		if ((target_pos_ - camera_lookat_).LengthSqr() >= 5) {
			move_state_ = MoveState::LERP;
			lerp_time_ = 0.0f;
			lerp_start_ = camera_lookat_;
		}
		break;
	case Camera::MoveState::LERP:
		lerp_time_ += dt;
		camera_lookat_.Lerp(lerp_start_, target_pos_, lerp_time_);
		if ((target_pos_ - camera_lookat_).LengthSqr() <= 0.01) {
			move_state_ = MoveState::TRACK;
			camera_lookat_ = target_pos_;
			prev_lookats_.push(camera_lookat_);
		}
		break;
	case Camera::MoveState::TRACK:
		camera_lookat_ = target_pos_;
		prev_lookats_.push(camera_lookat_);
		if (prev_lookats_.size() > 2) {
			gef::Vector4 prev_lookat_ = prev_lookats_.front();
			prev_lookats_.pop();
			if (effect_state_ == EffectState::NORMAL) {
				if (prev_lookat_.x() == camera_lookat_.x() && prev_lookat_.y() == camera_lookat_.y()) {
					move_state_ = MoveState::STATIONARY;
				}
			}
		}
		break;
	default:
		break;
	}

	camera_pos_ = camera_lookat_ + gef::Vector4(0, 0, 30);

	if (shake_time_ <= 0.0f) {
		effect_state_ = EffectState::NORMAL;
		shake_time_ = 0.2f;
	}
	shake_time_ -= dt;

	if (effect_state_ == EffectState::SHAKE) {
		if (move_state_ == MoveState::STATIONARY) camera_lookat_ = shake_start_lookat_;
		offset_ = gef::Vector4(0.05 * sin(100 * shake_time_), 0.05 * sin(80 * shake_time_), 0);
		camera_lookat_ += offset_;
	}
	else if (effect_state_ == EffectState::WARP) {
		offset_ = gef::Vector4(sin(40 * shake_time_), sin(40 * shake_time_), 0);
		camera_pos_ += offset_;
	}

	view_matrix_.SetIdentity();
	view_matrix_.LookAt(camera_pos_, camera_lookat_, up_);
}

void Camera::Shake() {
	if (effect_state_ != EffectState::SHAKE) {
		effect_state_ = EffectState::SHAKE; 
		shake_start_lookat_ = camera_lookat_;
	}
}

void Camera::SetPosition(gef::Vector4 pos) {
	camera_pos_ = pos; 
	camera_lookat_ = camera_pos_ - gef::Vector4(0, 0, 30);
}