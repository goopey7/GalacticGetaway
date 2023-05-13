#include "BulletManager.h"
#include <system/debug_log.h>

void BulletManager::Init(b2World* world, PrimitiveBuilder* builder) {
	world_ = world;
	builder_ = builder;
	for (int i = 0; i < 30; i++) {
		dead_bullets_.push_back(new Bullet);
		dead_bullets_.back()->Init(0.2, 0.2, 0.2, 0, 0, world_, builder_, true);
		dead_bullets_.back()->GetBody()->SetEnabled(false);
	}
}
 
void BulletManager::Update() {
	//std::list<Bullet*>::iterator bullet = live_bullets_.begin();

	for (int i=0; i < live_bullets_.size(); i++) {
		Bullet* bullet = live_bullets_[i];
		if (!bullet->GetBody()->IsAwake()) {
			bullet->GetBody()->SetEnabled(false);
			bullet->setAlive(false);
			if (dead_bullets_.size() < 30) {
				dead_bullets_.push_back(bullet);
				live_bullets_.erase(live_bullets_.begin() + i);
			}
			else {
				live_bullets_.erase(live_bullets_.begin() + i);
				delete bullet;
				bullet = nullptr;
			}
		}
		else {
			bullet->Update();
		}
		if(bullet != nullptr && bullet->TimeToDie())
		{
			live_bullets_.erase(live_bullets_.begin() + i);
			delete bullet;
			bullet = nullptr;
		}
	}

	//int deadNo = dead_bullets_.size();
	//if (deadNo > 30) {
	//	gef::DebugOut(("dead bullets: " + std::to_string(deadNo)).c_str());
	//	//std::list<Bullet*>::iterator bullet = dead_bullets_.;
	//}
}

void BulletManager::Fire(gef::Vector2 target_vector, gef::Vector2 start_pos, int damage, GameObject::Tag target, float speed) {
	if (dead_bullets_.empty()) {
		dead_bullets_.push_back(new Bullet);
		dead_bullets_.back()->Init(0.2, 0.2, 0.2, 0, 0, world_, builder_, true);
		dead_bullets_.back()->GetBody()->SetAwake(false);
	}
	live_bullets_.push_back(dead_bullets_.back());
	dead_bullets_.pop_back();
	live_bullets_.back()->Fire(target_vector, start_pos, damage, target, speed);
}

void BulletManager::Render(gef::Renderer3D* renderer_3d) const {
	for (Bullet* bullet : live_bullets_) {
		renderer_3d->DrawMesh(*bullet);
	}
}