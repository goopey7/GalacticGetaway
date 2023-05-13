#pragma once
#include "Bullet.h"
#include "graphics/renderer_3d.h"

class BulletManager {
public:
	void Init(b2World* world, PrimitiveBuilder* builder);
	void Update();
	void Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target, float speed = 10.f);
	void Render(gef::Renderer3D* renderer_3d) const;

protected:
	std::vector<Bullet*> live_bullets_;
	std::vector<Bullet*> dead_bullets_;

	b2World* world_;
	PrimitiveBuilder* builder_;
};

