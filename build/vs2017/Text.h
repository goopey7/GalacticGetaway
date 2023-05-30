#pragma once
#include <string>

#include "UIElement.h"
class Text : public UIElement
{
public:
	Text(const gef::Vector2& anchor, std::string text);
	Text(const gef::Vector2& anchor, std::string text, const gef::Platform& platform);
	void UpdateText(std::string text);
	std::string GetText() const;
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) const override;
	void SetAlpha(float alpha) override;
private:
	std::string text_;
	unsigned int alpha_color_ = 0xffffffff;
};
