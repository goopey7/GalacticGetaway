#include "Button.h"

#include <utility>

#include "InputActionManager.h"
#include "graphics/font.h"
#include "graphics/sprite_renderer.h"
#include "input/input_manager.h"
#include "input/touch_input_manager.h"
#include "system/platform.h"

Button::Button(const gef::Vector2& anchor, std::string text, gef::Colour color)
	: UIElement(anchor), text_(std::move(text)), color_(color)
{
	button.set_position(platform_->width() * anchor_.x, platform_->height() * anchor_.y, -0.9f);
	button.set_width(150);
	button.set_height(50);
	button.set_colour(color_.GetRGBA());
}

Button::Button(const gef::Vector2& anchor, const gef::Platform& platform, std::string text, gef::Colour color)
	: UIElement(anchor,platform), text_(std::move(text)), color_(color)
{
	button.set_position(platform_->width() * anchor_.x, platform_->height() * anchor_.y, -0.9f);
	button.set_width(150);
	button.set_height(50);
	button.set_colour(color_.GetRGBA());
}

void Button::Update(InputActionManager* iam, float frame_time)
{
	if(iam->getMousePos().x < button.position().x() + button.width() / 2.f &&
		iam->getMousePos().x > button.position().x() - button.width() / 2.f
	)
	{
		if(iam->getMousePos().y < button.position().y() + button.height() / 2.f &&
			iam->getMousePos().y > button.position().y() - button.height() / 2.f
		)
		{
			button.set_colour(0xff00ff00);
			if(iam->getInputManager()->touch_manager()->is_button_down(0)
				|| iam->isPressed(Jump))
			{
				OnClick_();
			}
		}
	}
	else
	{
		button.set_colour(0xffffffff);
	}
}

void Button::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	
	sprite_renderer->DrawSprite(button);
	font->RenderText(sprite_renderer, gef::Vector4(platform_->width() * anchor_.x, platform_->height() * anchor_.y - button.height() / 4.f, -0.9f), 1.0f, 0xff000000, gef::TJ_CENTRE,text_.c_str()); 
}
