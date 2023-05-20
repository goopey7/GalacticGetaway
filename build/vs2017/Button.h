#pragma once
#include <functional>
#include <string>

#include "UIElement.h"
#include "graphics/colour.h"
#include "graphics/sprite.h"

class Button : public UIElement
{
public:
	Button(const gef::Vector2& anchor, std::string text, gef::Colour color = gef::Colour(1, 1, 1, 1));
	Button(const gef::Vector2& anchor, const gef::Platform& platform, std::string text, gef::Colour color = gef::Colour(1,1,1,1));
	void SetOnClick(std::function<void()> onClick) { OnClick_ = onClick; }
	void Update(InputActionManager* iam, float frame_time) override;
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
private:
	std::string text_;
	gef::Colour color_;
	gef::Sprite button;
	std::function<void()> OnClick_;
};
