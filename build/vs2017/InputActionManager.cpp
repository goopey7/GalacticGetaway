﻿#include "InputActionManager.h"
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
	for(auto keyboardBinding : bindingsJson["actions"])
	{
		const Action action = stringToAction[keyboardBinding["action"]];
		for(const auto& key : keyboardBinding["keys"])
		{
			actionBindings[stringToKeyCode[key]] = action;
		}
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

	for(auto actionStr : actions)
	{
		Action action = stringToAction[actionStr];

		for(const auto binding : actionBindings)
		{
			if(action == binding.second)
			{
				bool oldMapPressed = actionMapPressed[action];
				bool oldMapHeld = actionMapHeld[action];
				bool oldMapReleased = actionMapReleased[action];
				
				actionMapPressed[action] = kb->IsKeyPressed(binding.first);
				actionMapHeld[action] = kb->IsKeyDown(binding.first);
				actionMapReleased[action] = kb->IsKeyReleased(binding.first);
				
				if(oldMapPressed != actionMapPressed[action] || oldMapHeld != actionMapHeld[action] || oldMapReleased != actionMapReleased[action])
				{
					break;
				}
			}
		}
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
