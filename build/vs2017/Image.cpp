#include "Image.h"

#include "graphics/sprite.h"
#include "graphics/sprite_renderer.h"
#include "system/platform.h"

Image::Image(const gef::Vector2& anchor, gef::Sprite* sprite, const gef::Platform& platform)
	: UIElement(anchor, platform), sprite_(sprite)
{
	sprite_->set_position(platform.width() * anchor_.x, platform.height() * anchor_.y, -0.9f);
}

void Image::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	sprite_renderer->DrawSprite(*sprite_);
}
