#pragma once
#include "GameObject.h"

class Bullet : public GameObject {
public:
	Bullet();
	void Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target, float speed = 10.f);
	void Update(float frame_time) override;
	void setAlive(bool b) { is_alive_ = b; }
	bool isAlive() { return is_alive_; }
	int getDamage() const { return damage_; }
	void setDamage(int damage) {damage_ = damage;}
	void BeginCollision(GameObject* other) override;
	Tag getTarget() const;
protected:
	bool is_alive_ = false;
	int damage_ = 0;
	float speed_ = 10.f;
	Tag target_ = Tag::None;
	bool bCollisionEnabled = true;
};

