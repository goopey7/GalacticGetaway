#pragma once
#include "Scene.h"

class UIElement;
class Image;

class SplashScreen : public Scene
{
public:
	SplashScreen(gef::Platform& platform, StateManager& state_manager) : Scene(platform, state_manager) {}
	void AddUIElement(UIElement* element);
	void Update(InputActionManager* iam, float frame_time) override;
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	void Render(gef::Renderer3D* renderer_3d) override {}
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font) override {Render(sprite_renderer, font);}
private:
	const float fade_in_time_ = 1.0f;
	const float fade_out_time_ = 1.0f;
	float fade_timer_ = 0.0f;
	float current_element_alpha_ = 0.f;
	std::queue<UIElement*> ui_elements_;
};
