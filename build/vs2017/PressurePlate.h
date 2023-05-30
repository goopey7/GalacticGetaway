#pragma once
#include <functional>
#include <set>

#include "GameObject.h"

namespace gef
{
	class Font;
}

class PressurePlate : public GameObject
{
public:
	void Init(gef::Vector4 size, gef::Vector4 pos, b2World* world, PrimitiveBuilder* builder, float threshold, gef::SpriteRenderer* sr, gef::Font* font, gef::Platform* platform, gef::AudioManager* am, float offset_y=0.f, bool is_fussy = false);
	void TraverseContactChain(GameObject* game_object, std::set<GameObject*>& visited_objects, float& total_weight);
	void Init(float size_x, float size_y, float size_z, float pos_x, float pos_y, b2World* world, PrimitiveBuilder* builder, gef::SpriteRenderer* sr, gef::Font* font, float threshold, gef::Platform* platform,gef::AudioManager* am, float offset_y=0.f, bool is_fussy = false);
	void Update(float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d) const override;
	void SetOnActivate(const std::function<void()>& on_activate) { on_activate_ = on_activate; }
	void SetOnDeactivate(const std::function<void()>& on_deactivate) { on_deactivate_ = on_deactivate; }
private:
	b2World* physics_world_ = nullptr;
	float threshold_ = 0.f;
	float current_load_ = 0.f;
	std::function<void()> on_activate_;
	std::function<void()> on_deactivate_;
	bool is_fussy_ = false;
	std::string hud_ = "0 / 1";
	gef::SpriteRenderer* sprite_renderer_ = nullptr;
	gef::Font* font_ = nullptr;
	gef::Platform* platform_ = nullptr;
	float offset_y_ = 0.f;
};
