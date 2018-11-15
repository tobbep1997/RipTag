#pragma once

#include <Multiplayer.h>
#include <map>
#include <functional>
#include <string.h>
#include <vector>
#include <list>
#include <sstream>

class Input
{
private:
	static bool m_deactivate;

	static DirectX::XMFLOAT2 m_mouseSensitivity;
	static float m_playerFOV;

public:
	Input();
	~Input();

	static Input * Instance();

	static void ForceDeactivateGamepad();
	static void ForceActivateGamepad();
	static void SetActivateGamepad(const bool & b);


	//Action Mappings
	static bool Jump();
	static bool CheckVisability();
	static bool Crouch();

	//Axis binds
	static float MoveForward();
	static float MoveRight();
	static float MouseMovementModifier();
	static bool ResetMouseMovementModifier();
	static float PeekRight();
	static bool Sprinting();
	static bool OnAbilityPressed();
	static bool OnAbilityReleased();
	static bool OnAbility2Pressed();
	static bool OnAbility2Released();
	static bool OnCancelAbilityPressed();
	static bool OnCancelAbilityReleased();
	static bool Interact();
	static bool Exit();
	static bool MouseLock();

	static float TurnUp();
	static float TurnRight();

	static bool isUsingGamepad();

	static void ResetMouse();

	static bool SelectAbility1();
	static bool SelectAbility2();
	static bool SelectAbility3();
	static bool SelectAbility4();

	//Player Settings----------------

	static const DirectX::XMFLOAT2 & GetPlayerMouseSensitivity();
	static int GetPlayerFOV();

	//LoadPlayerSettings from iniFile
	static void WriteSettingsToFile();
	static void ReadSettingsFile();
private:
	static void _ParseFileInputInt(const std::string & name, int key);
public:


};

class InputMapping
{
public:
	static std::map<std::string, std::function<void()>> functionMap;
	static std::map<int, std::string> keyMap;
	static std::map<int, std::string> devKeyMap; //This key map is reserved for keybinding for Dev tools

	//Gamepad maps -> the key is the function, the value is the string to use in the Network OnSend map
	static std::map<std::string, std::function<float()>> gamePadFunctionMapFloat;
	static std::map<std::string, std::function<bool()>> gamePadFunctionMapBool;
	//Mouse Map
	static std::map<std::string, std::function<bool()>> mouseFunctionMap;

	static void Init();
	static bool isInitialized;

	static void addToKeyMap(int key, std::string value);
	static void addToDevKeyMap(int key, std::string value);
	static void addToFuncMap(std::string key, std::function<void()> func);

	static void Call();
private:
	static void _ReloadKeyMapping();
	static void _LoadGamePadMapping();
	static void _LoadMouseMapping();
	static void _KeyboardCalls();
	static void _GamePadCalls();
	static void _MouseCalls();
};
