#pragma once

#include <vector>

#include "Scene.h"

class UIElement;

class Menu : public Scene
{
public:
	Menu(gef::Platform& platform, StateManager& state_manager, bool is_hud) : Scene(platform, state_manager), is_hud_(is_hud) {}
	void AddUIElement(UIElement* element);
	void Update(InputActionManager* iam, float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d) override {}
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font) override {Render(sprite_renderer, font);}
	void SetAlpha(float alpha);
private:
	std::vector<UIElement*> ui_elements_;
	UIElement* selected_element_ = nullptr;
	int selected_element_index_ = 0;
	std::vector<int> selectable_indices_;
	bool is_hud_ = false;
};
