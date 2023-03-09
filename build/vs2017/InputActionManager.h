#pragma once

#include "json.h"

using nlohmann::json;

#define DEFINE_ACTIONS(...) \
	enum Action { __VA_ARGS__ }; \
	const char* actionsStr = #__VA_ARGS__; \

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
	InputActionManager();
	void handleInputEvents();
	void initializeActions(const char* values);

private:
	json bindings;
	std::map<Action,bool> actionMapPressed;
	std::map<Action,bool> actionMapHeld;
	std::map<Action,bool> actionMapReleased;
	std::vector<std::string> actions;
};
