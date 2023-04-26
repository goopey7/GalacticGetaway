#pragma once
#include "GameObject.h"
#include "maths/vector2.h"

class Bullet : public GameObject {
public:
	Bullet();
	void Fire(gef::Vector2 target_vector, gef::Vector2 start_pos);
	void setAlive(bool b) { is_alive_ = b; }
	bool isAlive() { return is_alive_; }
protected:
	bool is_alive_ = false;
};

