#include "SplashScreen.h"

#include "Image.h"
#include "InputActionManager.h"
#include "UIElement.h"
#include "graphics/sprite_renderer.h"

void SplashScreen::AddUIElement(UIElement* element)
{
	element->SetAlpha(0.f);
	ui_elements_.push(element);
}

void SplashScreen::Update(InputActionManager* iam, float frame_time)
{
	if(ui_elements_.empty() || iam->isPressed(Action::Jump))
	{
		state_manager_->SetOnSplashScreen(false);
		state_manager_->SwitchToMainMenu(true);
		return;
	}
	
	ui_elements_.front()->SetAlpha(current_element_alpha_);
	fade_timer_ += frame_time;
	
	if(fade_timer_ < fade_in_time_)
	{
		current_element_alpha_ = fade_timer_ / fade_in_time_;
	}
	else if(fade_timer_ > fade_in_time_ + fade_out_time_)
	{
		current_element_alpha_ = 1.0f - ((fade_timer_ - fade_in_time_ - fade_out_time_) / fade_out_time_);
	}
	
	if(current_element_alpha_ < 0.0f)
	{
		ui_elements_.pop();
		fade_timer_ = 0.0f;
		current_element_alpha_ = 0.f;
	}
}

void SplashScreen::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	if(!ui_elements_.empty())
	{
		sprite_renderer->Begin(true);
		if(background_ != nullptr)
		{
			background_->Render(sprite_renderer, font);
		}
		ui_elements_.front()->Render(sprite_renderer, font);
		sprite_renderer->End();
	}
}

void SplashScreen::SetBkg(Image* image)
{
	background_ = image;
}
