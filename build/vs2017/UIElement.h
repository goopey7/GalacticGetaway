#pragma once
#include "maths/vector2.h"

namespace gef
{
	class Platform;
}

namespace gef
{
	class SpriteRenderer;
	class Font;
}

class InputActionManager;

class UIElement
{
public:
	UIElement(const gef::Vector2& anchor) : anchor_(anchor) {}
	UIElement(const gef::Vector2& anchor, const gef::Platform& platform) : anchor_(anchor) {SetPlatform(platform);}
	virtual void Render(gef::SpriteRenderer* sprite_renderer, gef::Font* font) = 0;
	virtual void Update(InputActionManager* iam, float frame_time) {}
	virtual void Interact() {}
	void SetPlatform(const gef::Platform& platform) {platform_ = &platform;}
	bool IsSelected();
	void Select();
	void Deselect();
	bool IsSelectable();
	virtual void SetAlpha(float alpha) = 0;

protected:
	const gef::Platform* platform_ = nullptr;
	gef::Vector2 anchor_;
	bool is_selected = false;
	bool is_selectable = false;
};
