#include "InputActionManager.h"
#include <fstream>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include <input/touch_input_manager.h>
#include "StringToGefInputEnum.h"

InputActionManager::InputActionManager(gef::Platform& platform)
{
	platform_ = &platform;

	// initialize actions vector from string
	initializeActions(actionsStr.c_str());

	// read bindings.json in config folder
	std::ifstream i("config/bindings.json");

	// handle error if file not found
	if (i.fail())
	{
		throw std::exception("bindings.json not found");
	}

	// parse json into cpp object
	bindingsJson = json::parse(i);

	// setup action bindings
	for (auto actionBinding : bindingsJson["actions"])
	{
		const Action action = stringToAction[actionBinding["action"]];
		for (const auto& key : actionBinding["keys"])
		{
			actionKeyBindings[stringToKeyCode[key]] = action;
		}

		for (const auto& controllerButton : actionBinding["buttons"])
		{
			actionControllerBindings[stringToControllerButton[controllerButton]] = action;
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

	auto controller = inputManager->controller_input()->GetController(0);

	inputManager->Update();

	for (auto actionStr : actions)
	{
		Action action = stringToAction[actionStr];

		bool breakOut = false;

		for (const auto binding : actionKeyBindings)
		{
			if (action == binding.second)
			{
				bool oldMapPressed = actionMapPressed[action];
				bool oldMapHeld = actionMapHeld[action];
				bool oldMapReleased = actionMapReleased[action];

				actionMapPressed[action] = kb->IsKeyPressed(binding.first);
				actionMapHeld[action] = kb->IsKeyDown(binding.first);
				actionMapReleased[action] = kb->IsKeyReleased(binding.first);

				if (kb->IsKeyPressed(binding.first)) {
					using_keyboard_ = true;
				}
				if (oldMapPressed != actionMapPressed[action] || oldMapHeld != actionMapHeld[action] || oldMapReleased != actionMapReleased[action])
				{

					breakOut = true;
					break;
				}
			}
		}

		if (breakOut)
		{
			continue;
		}

		for (const auto binding : actionControllerBindings)
		{
			if (action == binding.second)
			{
				bool oldMapPressed = actionMapPressed[action];
				bool oldMapHeld = actionMapHeld[action];
				bool oldMapReleased = actionMapReleased[action];

				actionMapPressed[action] = controller->buttons_pressed() & binding.first;
				actionMapHeld[action] = controller->buttons_down() & binding.first;
				actionMapReleased[action] = controller->buttons_released() & binding.first;

				if (controller->buttons_pressed() & binding.first) {
					using_keyboard_ = false;
				}
				if (oldMapPressed != actionMapPressed[action] || oldMapHeld != actionMapHeld[action] || oldMapReleased != actionMapReleased[action])
				{

					break;
				}
			}
		}
	}
	getLeftStickX();
	getLeftStickY();
	getRightStickX();
	getRightStickY();
	gef::Vector2 old_mouse_pos = mouse_pos_;
	mouse_pos_ = inputManager->touch_manager()->mouse_position();
	if (old_mouse_pos != mouse_pos_) using_keyboard_ = true;
}

float InputActionManager::getLeftStickX()
{
	float value = inputManager->controller_input()->GetController(0)->left_stick_x_axis();
	if (value) using_keyboard_ = false;
	return value;
}

float InputActionManager::getLeftStickY()
{
	float value = inputManager->controller_input()->GetController(0)->left_stick_y_axis();
	if (value) using_keyboard_ = false;
	return value;
}

float InputActionManager::getRightStickX()
{
	float value = inputManager->controller_input()->GetController(0)->right_stick_x_axis();
	if (value) using_keyboard_ = false;
	return value;
}

float InputActionManager::getRightStickY()
{
	float value = inputManager->controller_input()->GetController(0)->right_stick_y_axis();
	if (value) using_keyboard_ = false;
	return value;
}

void InputActionManager::initializeActions(const char* values)
{
	// parse string into vector of strings on the commas
	for (int i = 0; values[i] != '\0'; i++) {
		if (values[i] == ',') {
			actions.push_back(std::string(values, i));
			values = values + i + 1;
			i = -1;
		}
	}
	actions.push_back(values);

	// clear up spaces
	for (int i = 0; i < actions.size(); i++)
	{
		if (actions[i][0] == ' ')
		{
			actions[i] = actions[i].substr(1);
		}

		// setup string to action map for easy lookup
		stringToAction[actions[i]] = (Action)i;
	}
}
