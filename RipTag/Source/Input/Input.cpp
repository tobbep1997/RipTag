#include "Input.h"
#include "InputManager/InputHandler.h"
#include "InputManager/XboxInput/GamePadHandler.h"

bool Input::m_deactivate = false;

Input::Input()
{
}

Input::~Input()
{
}

void Input::ForceDeactivateGamepad()
{
	Input::m_deactivate = true;
}

void Input::ForceActivateGamepad()
{
	Input::m_deactivate = false;
}

bool Input::Jump()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		return GamePadHandler::IsAPressed();
	}
	else
	{
		if (InputHandler::isKeyPressed(InputHandler::Spacebar))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return 0;
}

float Input::MoveForward()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		return GamePadHandler::GetLeftStickYPosition();
	}
	else
	{
		if (InputHandler::isKeyPressed('W'))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed('S'))
		{
			return -1;
		}
	}
	return 0;
}

float Input::MoveRight()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		return GamePadHandler::GetLeftStickXPosition();
	}
	else
	{
		if (InputHandler::isKeyPressed('D'))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed('A'))
		{
			return -1;
		}
	}
	return 0;
}

float Input::PeekRight()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		float delta = 0.0f;

		delta = GamePadHandler::LeftTrigger() - GamePadHandler::RightTrigger();

		return delta;
	}
	else
	{
		if (InputHandler::isKeyPressed('Q'))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed('E'))
		{
			return -1;
		}
	}
	return 0;
}

float Input::TurnUp()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		return GamePadHandler::GetRightStickYPosition();
	}
	else
	{
		if (InputHandler::isKeyPressed(InputHandler::Up))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed(InputHandler::Down))
		{
			return -1;
		}
	}
	return 0;
}

float Input::TurnRight()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		return GamePadHandler::GetRightStickXPosition();
	}
	else
	{
		if (InputHandler::isKeyPressed(InputHandler::Right))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed(InputHandler::Left))
		{
			return -1;
		}
	}
	return 0;
}

std::map<std::string, std::function<void()>> InputMapping::functionMap;
std::map<char, std::string> InputMapping::keyMap;

void InputMapping::addToKeyMap(char key, std::string value)
{
	keyMap.insert(std::pair<char, std::string>(key, value));
}

void InputMapping::addToFuncMap(std::string key, std::function<void()> func)
{
	functionMap.insert(std::pair<std::string, std::function<void()>>(key, func));
}



void InputMapping::Call()
{
	std::map<char, std::string>::iterator keyIterator = keyMap.begin();
	for (keyIterator; keyIterator != keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			std::map<std::string, std::function<void()>>::iterator funcIterator;
			
			//find the function to call with the extracted key
			funcIterator = functionMap.find(keyIterator->second);
				//make sure it is mapped and found
			if (funcIterator != functionMap.end())
				funcIterator->second();
			
		}
	}
}
