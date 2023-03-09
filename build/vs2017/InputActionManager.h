#pragma once

#include "json.h"
#include "input/keyboard.h"

namespace gef
{
	class Platform;
	class InputManager;
}

using nlohmann::json;

#define DEFINE_ACTIONS(...) \
	enum Action { __VA_ARGS__ }; \
	const std::string actionsStr = {#__VA_ARGS__}; \

DEFINE_ACTIONS(
	MoveUp,
	MoveDown,
	MoveLeft,
	MoveRight,
	Jump,
)

class InputActionManager
{
public:
	explicit InputActionManager(gef::Platform& platform);
	bool isPressed(Action action);
	bool isHeld(Action action);
	bool isReleased(Action action);
	void Update();

private:
	void initializeActions(const char* values);
	json bindingsJson;
	std::map<gef::Keyboard::KeyCode, Action> actionBindings;
	std::map<Action,bool> actionMapPressed;
	std::map<Action,bool> actionMapHeld;
	std::map<Action,bool> actionMapReleased;
	std::vector<std::string> actions;
	std::map<std::string,Action> stringToAction;
	gef::InputManager* inputManager;
};
