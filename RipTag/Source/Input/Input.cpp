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

bool Input::CheckVisability()
{
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		GamePadHandler::IsLeftShoulderPressed();
	}
	else
	{
		if (InputHandler::isKeyPressed('C'))
		{
			return true;
		}
	}
	return false;
}

bool Input::Crouch()
{
	if (isUsingGamepad())
	{
		return GamePadHandler::IsRightStickPressed();
	}
	else
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Crouch")
				{
					return true;
				}
			}
		}
	}
	return false;
}

float Input::MoveForward()
{
	if (isUsingGamepad())
	{
		return GamePadHandler::GetLeftStickYPosition();
	}
	else
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "MoveForward")
				{
					return 1;
				}
				else if (keyIterator->second == "MoveBackward")
				{
					return -1;
				}
			}
		}
	}
	return 0;
}

float Input::MoveRight()
{
	if (isUsingGamepad())
	{
		return GamePadHandler::GetLeftStickXPosition();
	}
	else
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "MoveRight")
				{
					return 1;
				}
				else if (keyIterator->second == "MoveLeft")
				{
					return -1;
				}
			}
		}
	}
	return 0;
}

float Input::PeekRight()
{
	if (isUsingGamepad())
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

bool Input::Sprinting()
{
	if (isUsingGamepad())
		return GamePadHandler::IsLeftStickPressed();
	else
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Sprint")
				{
					return true;
				}
			}
		}
	}
	return false;
}

float Input::TurnUp()
{
	if (isUsingGamepad())
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
	if (isUsingGamepad())
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

bool Input::isUsingGamepad()
{
	return GamePadHandler::IsConnected() && m_deactivate == false;
}

std::map<std::string, std::function<void()>> InputMapping::functionMap;
std::map<int, std::string> InputMapping::keyMap;

void InputMapping::addToKeyMap(int key, std::string value)
{
	keyMap.insert(std::pair<int, std::string>(key, value));
}

void InputMapping::addToFuncMap(std::string key, std::function<void()> func)
{
	functionMap.insert(std::pair<std::string, std::function<void()>>(key, func));
}

void InputMapping::LoadKeyMapFromFile(std::string file)
{
	const int bufferSize = 1024;
	char buffer[bufferSize];
	if (GetPrivateProfileStringA("Keyboard", NULL, NULL, buffer, bufferSize, file.c_str()))
	{
		std::vector<std::string> nameList;
		std::istringstream nameStream;
		nameStream.str(std::string(buffer, bufferSize));

		std::string name = "";
		while (std::getline(nameStream, name, '\0'))
		{
			if (name == "")
				break;
			nameList.push_back(name);
		}

		for (size_t i = 0; i < nameList.size(); i++)
		{
			int key = -1;
			key = GetPrivateProfileIntA("Keyboard", nameList[i].c_str(), -1, file.c_str());
			if (key != -1)
				InputMapping::addToKeyMap(key, nameList[i]);
		}
	}
	else
	{
		std::cout << GetLastError() << std::endl;
	}

}



void InputMapping::Call()
{
	std::map<int, std::string>::iterator keyIterator = keyMap.begin();
	for (keyIterator; keyIterator != keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			std::map<std::string, std::function<void()>>::iterator inputFuncIterator;
			std::map<std::string, std::function<void()>>::iterator networkFuncIterator;
			
			//find the function to call with the extracted key
			inputFuncIterator = functionMap.find(keyIterator->second);
			networkFuncIterator = Network::Multiplayer::onSendMap.find(keyIterator->second);
				//make sure it is mapped and found
			if (inputFuncIterator != functionMap.end())
				inputFuncIterator->second();

			if (networkFuncIterator != Network::Multiplayer::onSendMap.end())
				networkFuncIterator->second();
			
		}
	}
}
