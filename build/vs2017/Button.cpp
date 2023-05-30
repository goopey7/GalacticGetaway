#include "Button.h"
#include <utility>
#include "InputActionManager.h"
#include "graphics/font.h"
#include "graphics/sprite_renderer.h"
#include "input/input_manager.h"
#include "input/touch_input_manager.h"
#include "system/platform.h"

Button::Button(const gef::Vector2& anchor, std::string text, float width, float height, gef::Colour color, gef::Colour hover_color, gef::Colour text_color)
	: UIElement(anchor), text_(std::move(text)), default_color_(color), hover_color_(hover_color), text_color_(text_color)
{
	is_selectable = true;
	button.set_position(platform_->width() * anchor_.x, platform_->height() * anchor_.y, -0.9f);
	button.set_width(width);
	button.set_height(height);
	button.set_colour(default_color_.GetRGBA());
}

Button::Button(const gef::Vector2& anchor, const gef::Platform& platform, std::string text, float width, float height, gef::Colour color, gef::Colour hover_color, gef::Colour text_color)
	: UIElement(anchor,platform), text_(std::move(text)), default_color_(color), hover_color_(hover_color), text_color_(text_color)
{
	is_selectable = true;
	button.set_position(platform_->width() * anchor_.x, platform_->height() * anchor_.y, -0.9f);
	button.set_width(width);
	button.set_height(height);
	button.set_colour(default_color_.GetRGBA());
}

void Button::Selected(InputActionManager* iam)
{
	button.set_colour(hover_color_.GetABGR());
	if(iam->isLMBPressed()
		|| iam->isPressed(MenuSelect))
	{
		Deselect();
		OnClick_();
	}
}

void Button::Update(InputActionManager* iam, float frame_time)
{
	if(is_selected)
	{
		Selected(iam);
	}
	else
	{
		button.set_colour(default_color_.GetABGR());
		if(iam->getMousePos().x < button.position().x() + button.width() / 2.f &&
			iam->getMousePos().x > button.position().x() - button.width() / 2.f
		)
		{
			if(iam->getMousePos().y < button.position().y() + button.height() / 2.f &&
				iam->getMousePos().y > button.position().y() - button.height() / 2.f)
			{
				is_selected = true;
			}
		}
		else
		{
			is_selected = false;
		}
	}
}

void Button::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) const
{
	sprite_renderer->DrawSprite(button);
	font->RenderText(sprite_renderer, gef::Vector4(platform_->width() * anchor_.x, platform_->height() * anchor_.y - button.height() / 4.f, -0.9f), 1.0f, text_color_.GetABGR(), gef::TJ_CENTRE,text_.c_str()); 
}

void Button::SetAlpha(float alpha)
{
	if (alpha > default_color_.a) return;
	button.set_colour(0x00ffffff | (static_cast<unsigned int>(alpha * 255) << 24));
	text_color_.SetFromAGBR(text_color_.GetABGR() | static_cast<unsigned int>(alpha * 255) << 24);
}
