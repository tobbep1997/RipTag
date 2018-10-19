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
	static bool Teleport();
	static bool Blink();

	static bool Exit();
	static bool MouseLock();

	static float TurnUp();
	static float TurnRight();

	static bool isUsingGamepad();

	static void ResetMouse();
};

class InputMapping
{
public:
	static std::map<std::string, std::function<void()>> functionMap;
	static std::map<int, std::string> keyMap;

	static void addToKeyMap(int key, std::string value);
	static void addToFuncMap(std::string key, std::function<void()> func);
	static void LoadKeyMapFromFile(std::string file);

	static void Call();
};
