#include "LoadingScreen.h"

#include "graphics/font.h"
#include "graphics/sprite_renderer.h"
#include "system/platform.h"

void LoadingScreen::SetStatusText(const char* text)
{
	status_text_ = text;
}

void LoadingScreen::Update(InputActionManager* iam, float frame_time)
{
	sprite_.set_position(gef::Vector4(platform_->width() / 2.f + offsets_[current_offset_], platform_->height() / 2.f, -0.9f));

	timer += frame_time;
	if(timer >= speed_)
	{
		if(animating_right_)
		{
			current_offset_++;
			if(current_offset_ >= offsets_.size())
			{
				current_offset_ = offsets_.size() - 2;
				animating_right_ = false;
			}
		}
		else
		{
			current_offset_--;
			if(current_offset_ < 0)
			{
				current_offset_ = 1;
				animating_right_ = true;
			}
		}
		timer = 0.f;
	}
}

void LoadingScreen::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	sprite_renderer->Begin(true);
	sprite_renderer->DrawSprite(sprite_);
	font->RenderText(sprite_renderer, gef::Vector4(platform_->width() / 2.f, platform_->height() / 2.f + 100.f, -0.9f), 1.0f, 0xffffffff, gef::TJ_CENTRE, status_text_);
	sprite_renderer->End();
}
