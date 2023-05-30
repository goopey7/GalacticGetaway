#pragma once
#include "UIElement.h"

namespace gef
{
	class Sprite;
}

class Image : public UIElement
{
public:
	Image(const gef::Vector2& anchor, gef::Sprite* sprite, const gef::Platform& platform);
	void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) const override;
	void SetAlpha(float alpha) override;
	gef::Sprite* GetSprite() { return sprite_; }
private:
	gef::Sprite* sprite_ = nullptr;
};
