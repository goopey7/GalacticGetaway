#pragma once

#include <vector>

#include "Scene.h"

class UIElement;

class Menu : public Scene
{
public:
	Menu(gef::Platform& platform) : Scene(platform) {}
	void AddUIElement(UIElement* element);
	void Update(InputActionManager* iam, float frame_time) override;
	void Render(gef::Renderer3D* renderer_3d) override {}
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
	void Render(gef::Renderer3D* renderer_3d, gef::SpriteRenderer* sprite_renderer, gef::Font* font) override {Render(sprite_renderer, font);}
private:
	std::vector<UIElement*> ui_elements_;
};
