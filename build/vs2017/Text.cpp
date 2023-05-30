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

Text::Text(const gef::Vector2& anchor, std::string text, const gef::Platform& platform)
	: UIElement(anchor, platform), text_(std::move(text))
{
}

void Text::UpdateText(std::string text)
{
	text_ = std::move(text);
}

std::string Text::GetText() const
{
	return text_;
}

void Text::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) const
{
	font->RenderText(sprite_renderer, gef::Vector4(platform_->width() * anchor_.x, platform_->height() * anchor_.y, -0.9f), 1.0f, alpha_color_, gef::TJ_CENTRE,text_.c_str()); 
}

void Text::SetAlpha(float alpha)
{
	alpha_color_ = 0x00ffffff | (static_cast<unsigned int>(alpha * 255) << 24);
}
