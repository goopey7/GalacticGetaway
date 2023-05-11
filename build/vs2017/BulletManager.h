#pragma once
#include "Bullet.h"
#include <list>
#include "graphics/renderer_3d.h"

class BulletManager {
public:
	void Init(b2World* world, PrimitiveBuilder* builder);
	void Update();
	void Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target);
	void Render(gef::Renderer3D* renderer_3d) const;

protected:
	std::list<Bullet*> live_bullets_;
	std::list<Bullet*> dead_bullets_;

	b2World* world_;
	PrimitiveBuilder* builder_;
};

