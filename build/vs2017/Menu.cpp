#include "Menu.h"

#include "InputActionManager.h"
#include "UIElement.h"
#include "graphics/sprite_renderer.h"

void Menu::AddUIElement(UIElement* element)
{
	element->SetPlatform(*platform_);
	ui_elements_.push_back(element);
}

void Menu::Update(InputActionManager* iam, float frame_time)
{
	// Check for mouse intersections with UI
	for (auto& element : ui_elements_)
	{
		element->Update(iam, frame_time);
	}

}

void Menu::Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font)
{
	// Render UI Elements
	sprite_renderer->Begin(true);
	for (auto& element : ui_elements_)
	{
		element->Render(sprite_renderer, font);
	}
	sprite_renderer->End();
}
