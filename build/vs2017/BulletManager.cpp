#include "BulletManager.h"

void BulletManager::Init(b2World* world, PrimitiveBuilder* builder) {
	world_ = world;
	builder_ = builder;
	for (int i = 0; i < 30; i++) {
		dead_bullets_.push_back(new Bullet);
		dead_bullets_.back()->Init(0.2, 0.2, 0.2, 0, 0, world_, builder_, true);
		dead_bullets_.back()->GetBody()->SetEnabled(false);
	}
}

//Need to do iteration manually so can remove safely 
void BulletManager::Update() {
	for (Bullet* bullet : live_bullets_) {
		if (!bullet->GetBody()->IsAwake()) {
			
			bullet->GetBody()->SetEnabled(false);
			bullet->setAlive(false);
			
			continue;
		}
		bullet->Update();
	}
}

void BulletManager::Fire(gef::Vector2 target_vector, gef::Vector2 start_pos) {
	if (dead_bullets_.empty()) {
		dead_bullets_.push_back(new Bullet);
		dead_bullets_.back()->Init(0.2, 0.2, 0.2, 0, 0, world_, builder_, true);
		dead_bullets_.back()->GetBody()->SetAwake(false);
	}
	live_bullets_.push_back(dead_bullets_.back());
	dead_bullets_.pop_back();
	live_bullets_.back()->Fire(target_vector, start_pos);
}

void BulletManager::Render(gef::Renderer3D* renderer_3d) {
	for (Bullet* bullet : live_bullets_) {
		renderer_3d->DrawMesh(*bullet);
	}
}