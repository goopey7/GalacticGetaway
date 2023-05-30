#pragma once
#include <functional>
#include <string>
#include "UIElement.h"
#include "graphics/colour.h"
#include "graphics/sprite.h"

class Button : public UIElement
{
public:
	Button(const gef::Vector2& anchor, std::string text, float width = 150.f, float height = 50.f, gef::Colour color = gef::Colour(1, 1, 1, 1), gef::Colour hover_color = gef::Colour(1, 1, 1, 1), gef::Colour text_color = gef::Colour(0,0,0,1));
	Button(const gef::Vector2& anchor, const gef::Platform& platform, std::string text, float width = 150.f, float height = 50.f, gef::Colour color = gef::Colour(1,1,1,1), gef::Colour hover_color = gef::Colour(1, 1, 1, 1), gef::Colour text_color = gef::Colour(0,0,0,1));
	void Selected(InputActionManager* iam);
	void SetOnClick(std::function<void()> onClick) { OnClick_ = onClick; }
	void Update(InputActionManager* iam, float frame_time) override;
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) const override;
	gef::Sprite* GetSprite() {return &button;}
	void SetText(std::string text) {text_ = std::move(text);}
	void SetAlpha(float alpha) override;
private:
	std::string text_;
	gef::Colour default_color_;
	gef::Colour hover_color_;
	gef::Colour text_color_;
	gef::Sprite button;
	std::function<void()> OnClick_;
};
