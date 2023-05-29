#pragma once

#include "json.h"
#include "input/keyboard.h"
#include <maths/vector2.h>

namespace gef
{
	class Platform;
	class InputManager;
}

using json = nlohmann::json;

#define DEFINE_ACTIONS(...) \
	enum Action { __VA_ARGS__ }; \
	const std::string actionsStr = {#__VA_ARGS__}; \

DEFINE_ACTIONS(
	MoveLeft,
	MoveRight,
	Jump,
	GravityLock,
	GravityUp,
	GravityDown,
	GravityLeft,
	GravityRight,
	GravityStrenghtUp,
	GravityStrengthDown,
	Fire,
	Grapple,
	Reload,
	Pause,
	MenuSelect,
	MenuSelectRight,
	MenuSelectLeft,
	UseItem
	)

	class InputActionManager
{
public:
	explicit InputActionManager(gef::Platform& platform);
	bool isPressed(Action action);
	bool isLMBPressed();
	bool isRMBPressed();
	bool isHeld(Action action);
	bool isReleased(Action action);
	void Update();

	float getLeftStickX();
	float getLeftStickY();
	float getRightStickX();
	float getRightStickY();
	gef::Vector2 getMousePos() { return mouse_pos_; }
	gef::InputManager* getInputManager() { return inputManager; }

	bool getUsingKeyboard() { return using_keyboard_; }

private:
	void initializeActions(const char* values);
	json bindingsJson;
	std::map<gef::Keyboard::KeyCode, std::vector<Action>> actionKeyBindings;
	std::map<uint32_t, std::vector<Action>> actionControllerBindings;
	std::map<Action, bool> actionMapPressed;
	std::map<Action, bool> actionMapHeld;
	std::map<Action, bool> actionMapReleased;
	std::vector<std::string> actions;
	std::map<std::string, Action> stringToAction;
	gef::InputManager* inputManager;
	gef::Platform* platform_;
	gef::Vector2 mouse_pos_;
	bool using_keyboard_ = true;
	bool lmb_pressed_ = false;
	bool rmb_pressed_ = false;
};
