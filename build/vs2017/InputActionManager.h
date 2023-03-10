#pragma once

#include "json.h"
#include "input/keyboard.h"

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
	Quit,
)

class InputActionManager
{
public:
	explicit InputActionManager(gef::Platform& platform);
	bool isPressed(Action action);
	bool isHeld(Action action);
	bool isReleased(Action action);
	void Update();

	float getLeftStickX();
	float getLeftStickY();
	float getRightStickX();
	float getRightStickY();

private:
	void initializeActions(const char* values);
	json bindingsJson;
	std::map<gef::Keyboard::KeyCode, Action> actionKeyBindings;
	std::map<uint32_t, Action> actionControllerBindings;
	std::map<Action,bool> actionMapPressed;
	std::map<Action,bool> actionMapHeld;
	std::map<Action,bool> actionMapReleased;
	std::vector<std::string> actions;
	std::map<std::string,Action> stringToAction;
	gef::InputManager* inputManager;
};
