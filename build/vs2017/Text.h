#pragma once
#include <string>

#include "UIElement.h"
class Text : public UIElement
{
public:
	Text(const gef::Vector2& anchor, std::string text);
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
private:
	std::string text_;
};
