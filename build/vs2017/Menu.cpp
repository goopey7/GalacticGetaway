#include "Menu.h"

#include "InputActionManager.h"
#include "UIElement.h"
#include "graphics/sprite_renderer.h"

void Menu::AddUIElement(UIElement* element)
{
	element->SetPlatform(*platform_);
	ui_elements_.push_back(element);
	if(element->IsSelectable())
	{
		selectable_indices_.push_back(ui_elements_.size() - 1);
		selected_element_index_ = selectable_indices_[0];
		ui_elements_[selected_element_index_]->Select();
	}
}

void Menu::Update(InputActionManager* iam, float frame_time)
{
	// Check if anything else has been selected and if so deselect what was selected
	for (int i = 0; i < selectable_indices_.size(); i++)
	{
		const auto& element = ui_elements_[selectable_indices_[i]];
		
		if (element->IsSelected() && element != selected_element_)
		{
			if(selected_element_ != nullptr)
			{
				selected_element_->Deselect();
			}
			selected_element_ = element;
			selected_element_index_ = i;
			break;
		}
	}
	
	// Check for intersections with UI
	for (auto& element : ui_elements_)
	{
		element->Update(iam, frame_time);
	}

	if(iam->isPressed(MenuSelectRight))
	{
		int indexToSelect = selected_element_index_ + 1;
		if(indexToSelect >= selectable_indices_.size())
		{
			indexToSelect = 0;
		}
		ui_elements_[selectable_indices_[indexToSelect]]->Select();
	}

	if(iam->isPressed(MenuSelectLeft))
	{
		int indexToSelect = selected_element_index_ - 1;
		if(indexToSelect < 0)
		{
			indexToSelect = selectable_indices_.size() - 1;
		}
		ui_elements_[selectable_indices_[indexToSelect]]->Select();
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
