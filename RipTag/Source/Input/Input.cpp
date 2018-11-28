#include "RipTagPCH.h"
#include "Input.h"

bool Input::m_deactivate = false;
DirectX::XMFLOAT2 Input::m_mouseSensitivity = {0,0};
float Input::m_playerFOV = 0;

Input::Input()
{
}

Input::~Input()
{
}

Input* Input::Instance()
{
	static Input instance;
	return &instance;
}

void Input::ForceDeactivateGamepad()
{
	Input::m_deactivate = true;
}

void Input::ForceActivateGamepad()
{
	Input::m_deactivate = false;
}

void Input::SetActivateGamepad(const bool & b)
{
	Input::m_deactivate = b;
}

bool Input::Jump()
{
	bool result = false;

	if (isUsingGamepad())
		result = GamePadHandler::IsAPressed();
	
	if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Jump")
				{
					result = true;
				}
			}
		}
	}
	
	return result;
}

bool Input::CheckVisability()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsLeftShoulderPressed();

	if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Visibility")
				{
					result =  true;
				}
			}
		}
	}
	return result;
}

bool Input::Crouch()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsRightStickPressed();
	
	if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Crouch")
				{
					result = true;
				}
			}
		}
	}
	
	return result;
}


bool Input::Interact()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsXPressed();
	
	if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Interact")
				{
					result = true;
				}
			}
		}
	}
	
	return result;
}

float Input::MoveForward()
{
	float result = 0;
	if (isUsingGamepad())
		result = GamePadHandler::GetLeftStickYPosition();
	
	if (result == 0)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "MoveForward")
				{
					result = 1.1;
				}
				else if (keyIterator->second == "MoveBackward")
				{
					result = -1.1;
				}
			}
		}
	}
	
	return result;
}

float Input::MoveRight()
{
	float result = 0;
	if (isUsingGamepad())
		result = GamePadHandler::GetLeftStickXPosition();

	if (result == 0)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "MoveRight")
				{
					result = 1.1;
				}
				else if (keyIterator->second == "MoveLeft")
				{
					result = -1.1;
				}
			}
		}
	}
	return result;
}

float Input::MouseMovementModifier()
{
	return InputHandler::getMouseDelta();
}

bool Input::ResetMouseMovementModifier()
{
	return InputHandler::isMMiddlePressed();
}

float Input::PeekRight()
{
	float result = 0;
	if (isUsingGamepad())
		result = GamePadHandler::LeftTrigger() - GamePadHandler::RightTrigger();

	if (result == 0)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "PeekRight")
				{
					result = -1;
				}
				else if (keyIterator->second == "PeekLeft")
				{
					result = 1;
				}
			}
		}
	}
	return result;
}

bool Input::Sprinting()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsLeftStickPressed();

	if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "Sprint")
				{
					result = true;
				}
			}
		}
	}
	
	return result;
}

bool Input::OnAbilityPressed()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsRightShoulderPressed();
	
	if (!result)
		result = InputHandler::isMouseLeftPressed();
	/*if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "AbilityPressed")
				{
					result = true;
				}
			}
		}
	}*/

	return result;
}

bool Input::OnAbilityReleased()
{
	static bool previousFrame = false;
	if (Input::OnAbilityPressed())
		previousFrame = true;
	if (!Input::OnAbilityPressed() && previousFrame)
	{
		previousFrame = false;
		return true;
	}
	return false;
}

bool Input::OnAbility2Pressed()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsLeftShoulderPressed();

	if (!result)
		result = InputHandler::isMRightPressed();
	/*if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "AbilityPressed")
				{
					result = true;
				}
			}
		}
	}*/

	return result;
}

bool Input::OnAbility2Released()
{
	static bool previousFrame = false;
	if (Input::OnAbility2Pressed())
		previousFrame = true;
	if (!Input::OnAbility2Pressed() && previousFrame)
	{
		previousFrame = false;
		return true;
	}
	return false;
}

bool Input::OnCancelAbilityPressed()
{
	bool result = false;
	if (isUsingGamepad())
		result = GamePadHandler::IsBPressed();

	if (!result)
	{
		std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
		for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
		{
			if (InputHandler::isKeyPressed(keyIterator->first))
			{
				if (keyIterator->second == "CancelAbility")
				{
					result = true;
				}
			}
		}
	}

	return result;
}

bool Input::OnCancelAbilityReleased()
{
	static bool previousFrame = false;
	if (Input::OnCancelAbilityPressed())
		previousFrame = true;
	if (!Input::OnCancelAbilityPressed() && previousFrame)
	{
		previousFrame = false;
		return true;
	}
	return false;
}

bool Input::Exit()
{
	std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
	for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "Exit")
			{
				return true;
			}
		}
	}
	return false;
}

bool Input::MouseLock()
{
	std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
	for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "UnlockMouse")
			{
				return true;
			}
		}
	}
	return false;
}

float Input::TurnUp()
{
	float result = 0;
	if (isUsingGamepad())
		result = -20.0f * GamePadHandler::GetRightStickYPosition();
	if (result == 0)
	{
		DirectX::XMFLOAT2 poss = InputHandler::getMousePosition();
		//result = -1.0f * (((InputHandler::getWindowSize().y / 2)) - poss.y) / 40.0f;
		float standardY = InputHandler::getWindowSize().y / 2;
		result = -1 * (standardY - poss.y);
	}
	return result;
}

float Input::TurnRight()
{
	float result = 0;
	if (isUsingGamepad())
		result = 20.0f * GamePadHandler::GetRightStickXPosition();

	if (result == 0)
	{
		DirectX::XMFLOAT2 poss = InputHandler::getMousePosition();
		//result = -1.0f * (((InputHandler::getWindowSize().x / 2.0f)) - poss.x) / 40.0f;
		float standardY = InputHandler::getWindowSize().x / 2;
		result = -1 * (standardY - poss.x);
	}
	return result;
}

bool Input::isUsingGamepad()
{
	return GamePadHandler::IsConnected() && m_deactivate == false;
}

//DEFINITIONS FOR INPUTMAPPING STATICS
std::map<std::string, std::function<void()>> InputMapping::functionMap;
std::map<int, std::string> InputMapping::keyMap;
std::map<int, std::string> InputMapping::devKeyMap;
std::map<std::string, std::function<float()>> InputMapping::gamePadFunctionMapFloat;
std::map<std::string, std::function<bool()>> InputMapping::gamePadFunctionMapBool;
std::map<std::string, std::function<bool()>> InputMapping::mouseFunctionMap;

bool InputMapping::isInitialized = false;


void InputMapping::Init()
{
	if (!isInitialized)
	{
		InputMapping::_LoadGamePadMapping();
		InputMapping::_LoadMouseMapping();

		std::string file = "..\\Configuration\\KeyMapping.ini";
		const int bufferSize = 1024;
		char buffer[bufferSize];

			//Load in Keyboard section
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
			//Clear buffer for reuse
			ZeroMemory(buffer, bufferSize);

		}
		else
			std::cout << GetLastError() << std::endl;
		//Load in Dev section (Keybindings for Dev Tools)
		if (GetPrivateProfileStringA("Dev", NULL, NULL, buffer, bufferSize, file.c_str()))
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
				key = GetPrivateProfileIntA("Dev", nameList[i].c_str(), -1, file.c_str());
				if (key != -1)
					InputMapping::addToDevKeyMap(key, nameList[i]);
			}
			//Clear buffer for reuse
			ZeroMemory(buffer, bufferSize);
		}
		else
			std::cout << GetLastError() << std::endl;

		isInitialized = true;
	}
}

void InputMapping::addToKeyMap(int key, std::string value)
{
	keyMap.insert(std::pair<int, std::string>(key, value));
}

void InputMapping::addToDevKeyMap(int key, std::string value)
{
	devKeyMap.insert(std::pair<int, std::string>(key, value));
}

void InputMapping::addToFuncMap(std::string key, std::function<void()> func)
{
	functionMap.insert(std::pair<std::string, std::function<void()>>(key, func));
}

void InputMapping::Call()
{
	//Reload if we pressed the assigned key
	_ReloadKeyMapping();
	_KeyboardCalls();
	_GamePadCalls();
	_MouseCalls();
}

void InputMapping::_ReloadKeyMapping()
{
	std::map<int, std::string>::iterator keyIterator = devKeyMap.begin();
	for (keyIterator; keyIterator != devKeyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "ReloadKeyMapping")
			{
				//clear all of our keymaps
				keyMap.clear();
				devKeyMap.clear();
				//Load them in with Init again
				isInitialized = false;
				InputMapping::Init();
				return;
			}
		}
	}
}

void InputMapping::_LoadGamePadMapping()
{
	if (gamePadFunctionMapFloat.size() == 0 && gamePadFunctionMapBool.size() == 0)
	{
		//Gamepad bindings are hardcoded and unchangeable by the user
		//FLOAT FUNCTIONS
		gamePadFunctionMapFloat.insert(std::pair<std::string, std::function<float()>>("MoveForward", std::bind(&GamePadHandler::GetLeftStickYPosition)));
		gamePadFunctionMapFloat.insert(std::pair<std::string, std::function<float()>>("MoveRight", std::bind(&GamePadHandler::GetLeftStickXPosition)));

		//BOOL FUNCTION
		gamePadFunctionMapBool.insert(std::pair<std::string, std::function<bool()>>("Jump", std::bind(&GamePadHandler::IsAPressed)));
		gamePadFunctionMapBool.insert(std::pair<std::string, std::function<bool()>>("AbilityPressed", std::bind(&GamePadHandler::IsRightShoulderPressed)));
		gamePadFunctionMapBool.insert(std::pair<std::string, std::function<bool()>>("AbilityReleased", std::bind(&GamePadHandler::IsRightShoulderReleased)));
		gamePadFunctionMapBool.insert(std::pair<std::string, std::function<bool()>>("Ability2Pressed", std::bind(&GamePadHandler::IsLeftShoulderPressed)));
		gamePadFunctionMapBool.insert(std::pair<std::string, std::function<bool()>>("Ability2Released", std::bind(&GamePadHandler::IsLeftShoulderReleased)));
	}
}

void InputMapping::_LoadMouseMapping()
{
	mouseFunctionMap.insert(std::pair<std::string, std::function<bool()>>("AbilityPressed", std::bind(&InputHandler::isMouseLeftPressed)));
	mouseFunctionMap.insert(std::pair<std::string, std::function<bool()>>("AbilityReleased", std::bind(&InputHandler::isMLeftReleased)));
	mouseFunctionMap.insert(std::pair<std::string, std::function<bool()>>("Ability2Pressed", std::bind(&InputHandler::isMRightPressed)));
	mouseFunctionMap.insert(std::pair<std::string, std::function<bool()>>("Ability2Released", std::bind(&InputHandler::isMRightReleased)));
}

void InputMapping::_KeyboardCalls()
{
	std::map<int, std::string>::iterator keyIterator = keyMap.begin();
	for (keyIterator; keyIterator != keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			std::map<std::string, std::function<void()>>::iterator networkFuncIterator;
			networkFuncIterator = Network::Multiplayer::LocalPlayerOnSendMap.find(keyIterator->second);

			if (networkFuncIterator != Network::Multiplayer::LocalPlayerOnSendMap.end())
				networkFuncIterator->second();

		}
	}
}

void InputMapping::_GamePadCalls()
{
	//check if we have input from gamepad. If true then call the mapped function from the OnSend map and Input map
	std::map<std::string, std::function<float()>>::iterator floatMapIterator = gamePadFunctionMapFloat.begin();
	for (floatMapIterator; floatMapIterator != gamePadFunctionMapFloat.end(); floatMapIterator++)
	{
		if (floatMapIterator->second() != 0.0f)
		{
			std::map<std::string, std::function<void()>>::iterator networkFuncIterator;
			networkFuncIterator = Network::Multiplayer::LocalPlayerOnSendMap.find(floatMapIterator->first);

			if (networkFuncIterator != Network::Multiplayer::LocalPlayerOnSendMap.end())
				networkFuncIterator->second();
		}
	}
	//do the same thing for the bool map
	std::map<std::string, std::function<bool()>>::iterator boolMapIterator = gamePadFunctionMapBool.begin();
	for (boolMapIterator; boolMapIterator != gamePadFunctionMapBool.end(); boolMapIterator++)
	{
		if (boolMapIterator->second())
		{
			std::map<std::string, std::function<void()>>::iterator networkFuncIterator;
			networkFuncIterator = Network::Multiplayer::LocalPlayerOnSendMap.find(boolMapIterator->first);

			if (networkFuncIterator != Network::Multiplayer::LocalPlayerOnSendMap.end())
				networkFuncIterator->second();
		}
	}
}

void InputMapping::_MouseCalls()
{
	std::map<std::string, std::function<bool()>>::iterator boolMapIterator = mouseFunctionMap.begin();
	for (boolMapIterator; boolMapIterator != mouseFunctionMap.end(); boolMapIterator++)
	{
		if (boolMapIterator->second())
		{
			std::map<std::string, std::function<void()>>::iterator networkFuncIterator;
			networkFuncIterator = Network::Multiplayer::LocalPlayerOnSendMap.find(boolMapIterator->first);

			if (networkFuncIterator != Network::Multiplayer::LocalPlayerOnSendMap.end())
				networkFuncIterator->second();
		}
	}
}

void Input::ResetMouse()
{
	if (InputHandler::getWindowFocus())
	{
		int midX = (int)InputHandler::getviewportPos().x + (int)(InputHandler::getWindowSize().x / 2.0f);
		int midY = (int)InputHandler::getviewportPos().y + (int)(InputHandler::getWindowSize().y / 2.0f);

		SetCursorPos(midX, midY);
	}
}

bool Input::SelectAbility1()
{
	std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
	for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "SelectAbility1")
			{
				return true;
			}
		}
	}
	return false;
}

bool Input::SelectAbility2()
{
	std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
	for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "SelectAbility2")
			{
				return true;
			}
		}
	}
	return false;
}

bool Input::SelectAbility3()
{
	std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
	for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "SelectAbility3")
			{
				return true;
			}
		}
	}
	return false;
}

bool Input::SelectAbility4()
{
	std::map<int, std::string>::iterator keyIterator = InputMapping::keyMap.begin();
	for (keyIterator; keyIterator != InputMapping::keyMap.end(); keyIterator++)
	{
		if (InputHandler::isKeyPressed(keyIterator->first))
		{
			if (keyIterator->second == "SelectAbility4")
			{
				return true;
			}
		}
	}
	return false;
}

const DirectX::XMFLOAT2 & Input::GetPlayerMouseSensitivity()
{
	return m_mouseSensitivity;
}

int Input::GetPlayerFOV()
{
	return m_playerFOV;
}

void Input::ReadSettingsFile()
{
	std::string file = "..\\Configuration\\PlayerMapping.ini";
	const int bufferSize = 1024;
	char buffer[bufferSize];

	//Load in Keyboard section
	if (GetPrivateProfileStringA("Player", NULL, NULL, buffer, bufferSize, file.c_str()))
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
			key = GetPrivateProfileIntA("Player", nameList[i].c_str(), -1, file.c_str());
			if (key != -1)
			{
				_ParseFileInputInt(nameList[i], key);
			}
				
		}
		//Clear buffer for reuse
		ZeroMemory(buffer, bufferSize);

	}
	else
		std::cout << GetLastError() << std::endl;
}

void Input::_ParseFileInputInt(const std::string& name, int key)
{
	if (name == "XAXIS")
	{
		m_mouseSensitivity.x = key;
	}
	else if (name == "YAXIS")
	{
		m_mouseSensitivity.y = key;
	}
	else if (name == "PlayerFOV")
	{
		m_playerFOV = key;
	}
}


//DEFINITIONS FOR INPUTMAPPING STATICS
