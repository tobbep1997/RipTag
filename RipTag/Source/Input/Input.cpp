#include "Input.h"
#include "../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../../InputManager/InputHandler.h"

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

