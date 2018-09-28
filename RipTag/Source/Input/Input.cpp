#include "Input.h"
#include "../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../../InputManager/InputHandler.h"

Input::Input()
{
}

Input::~Input()
{
}

float Input::MoveForward()
{
	if (GamePadHandler::IsConnected())
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
	if (GamePadHandler::IsConnected())
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

float Input::TurnUp()
{
	if (GamePadHandler::IsConnected())
	{
		return GamePadHandler::GetRightStickYPosition();
	}
	else
	{
		if (InputHandler::isKeyPressed(InputHandler::UpArrow))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed(InputHandler::DownArrow))
		{
			return -1;
		}
	}
	return 0;
}

float Input::TurnRight()
{
	if (GamePadHandler::IsConnected())
	{
		return GamePadHandler::GetRightStickXPosition();
	}
	else
	{
		if (InputHandler::isKeyPressed(InputHandler::RightArrow))
		{
			return 1;
		}
		else if (InputHandler::isKeyPressed(InputHandler::LeftArrow))
		{
			return -1;
		}
	}
	return 0;
}

