#include "Text.h"

#include <utility>

#include "maths/vector4.h"
#include "graphics/font.h"
#include "graphics/sprite_renderer.h"
#include "system/platform.h"

Text::Text(const gef::Vector2& anchor, std::string text)
	: UIElement(anchor), text_(std::move(text))
{
}

void Text::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	font->RenderText(sprite_renderer, gef::Vector4(platform_->width() * anchor_.x, platform_->height() * anchor_.y, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE,text_.c_str()); 
}
