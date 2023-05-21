#include "UIElement.h"

bool UIElement::IsSelected()
{
	return is_selected;
}

void UIElement::Select()
{
	is_selected = true;
}

void UIElement::Deselect()
{
	is_selected = false;
}

bool UIElement::IsSelectable()
{
	return is_selectable;
}
