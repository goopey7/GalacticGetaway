#pragma once
#include <functional>
#include <set>

#include "GameObject.h"

class PressurePlate : public GameObject
{
public:
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, float threshold);
	void TraverseContactChain(GameObject* game_object, std::set<GameObject*>& visited_objects, float& total_weight);
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, float threshold);
	void Update(float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d, PrimitiveBuilder* builder) const override;
	void SetOnActivate(const std::function<void()>& on_activate) { on_activate_ = on_activate; }
	void SetOnDeactivate(const std::function<void()>& on_deactivate) { on_deactivate_ = on_deactivate; }
private:
	b2World* physics_world_ = nullptr;
	float threshold_ = 0.f;
	float current_load_ = 0.f;
	std::function<void()> on_activate_;
	std::function<void()> on_deactivate_;
};
