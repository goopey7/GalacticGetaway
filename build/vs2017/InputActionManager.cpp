#include "InputActionManager.h"
#include <fstream>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include "StringToKeyCode.h"

InputActionManager::InputActionManager(gef::Platform& platform)
{
	// initialize actions vector from string
	initializeActions(actionsStr.c_str());

	// TODO handle error if file not found
	
	// read bindings.json in config folder
	std::ifstream i("config/bindings.json");

	// parse json into cpp object
	bindingsJson = json::parse(i);

	// setup action bindings
	for(auto keyboardBinding : bindingsJson["keyboard"])
	{
		actionBindings[stringToKeyCode[keyboardBinding["key"]]] = stringToAction[keyboardBinding["action"]];
	}

	inputManager = gef::InputManager::Create(platform);
}

bool InputActionManager::isPressed(Action action)
{
	return actionMapPressed[action];
}

bool InputActionManager::isHeld(Action action)
{
	return actionMapHeld[action];
}

bool InputActionManager::isReleased(Action action)
{
	return actionMapReleased[action];
}

void InputActionManager::Update()
{
	auto kb = inputManager->keyboard();
	kb->Update();
	
	for(auto binding : actionBindings)
	{
		actionMapPressed[binding.second] = kb->IsKeyPressed(binding.first);
		actionMapHeld[binding.second] = kb->IsKeyDown(binding.first);
		actionMapReleased[binding.second] = kb->IsKeyReleased(binding.first);
	}
}

void InputActionManager::initializeActions(const char* values)
{
	// parse string into vector of strings on the commas
	for(int i = 0; values[i] != '\0'; i++) {
		if(values[i] == ',') {
			actions.push_back(std::string(values, i));
			values = values + i + 1;
			i = -1;
		}
	}
	actions.push_back(values);

	// clear up spaces
	for(int i = 0; i < actions.size(); i++)
	{
		if(actions[i][0] == ' ')
		{
			actions[i] = actions[i].substr(1);
		}

		// setup string to action map for easy lookup
		stringToAction[actions[i]] = (Action)i;
	}
}
