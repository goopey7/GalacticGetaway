#pragma once
#include <string>

#include "UIElement.h"
class Text : public UIElement
{
public:
	Text(const gef::Vector2& anchor, std::string text);
	Text(const gef::Vector2& anchor, std::string text, const gef::Platform& platform);
	void UpdateText(std::string text);
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) override;
private:
	std::string text_;
};
