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


	static float TurnUp();
	static float TurnRight();
};

class InputMapping
{
public:
	
	static std::map<std::string, std::function<void()>> functionMap;
	static std::map<char, std::string> keyMap;

	static void addToKeyMap(char key, std::string value);
	static void addToFuncMap(std::string key, std::function<void()> func);
	static void LoadKeyMapFromFile(std::string file);

	static void Call();
};
