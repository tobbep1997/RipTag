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
		if (InputHandler::isKeyPressed(InputHandler::SPACEBAR))
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
	if (GamePadHandler::IsConnected() && m_deactivate == false)
	{
		return GamePadHandler::IsRightStickPressed();
	}
	else
	{
		if (InputHandler::isKeyPressed('C'))
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
	if (GamePadHandler::IsConnected() && m_deactivate == false)
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

