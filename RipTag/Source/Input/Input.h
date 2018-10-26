#pragma once

#include <Multiplayer.h>
#include <map>
#include <functional>
#include <string>
#include <list>

class Input
{
	static bool m_deactivate;

public:
	Input();
	~Input();

	static void ForceDeactivateGamepad();
	static void ForceActivateGamepad();


	//Action Mappings
	static bool Jump();
	static bool CheckVisability();
	static bool Crouch();

	//Axis binds
	static float MoveForward();
	static float MoveRight();
	static float PeekRight();
	static bool Sprinting();
	static bool UseAbility();
	static bool Blink();
	static bool Possess();

	static bool Exit();
	static bool MouseLock();

	static float TurnUp();
	static float TurnRight();

	static bool isUsingGamepad();

	static void ResetMouse();

	static bool SelectAbility1();
	static bool SelectAbility2();
	static bool SelectAbility3();
};

class InputMapping
{
public:
	static std::map<std::string, std::function<void()>> functionMap;
	static std::map<int, std::string> keyMap;
	static std::map<int, std::string> devKeyMap; //This key map is reserved for keybinding for Dev tools

	static void Init();
	static bool isInitialized;

	static void addToKeyMap(int key, std::string value);
	static void addToDevKeyMap(int key, std::string value);
	static void addToFuncMap(std::string key, std::function<void()> func);

	static void Call();
private:
	static void _ReloadKeyMapping();
};
