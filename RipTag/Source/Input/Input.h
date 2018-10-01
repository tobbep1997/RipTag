#pragma once

class Input
{
	static bool m_deactivate;

public:
	Input();
	~Input();

	static void ForceDeactivateGamepad();
	static void ForceActivateGamepad();


	//Action Mappings

	//Axis binds
	static float MoveForward();
	static float MoveRight();

	static float TurnUp();
	static float TurnRight();
};
