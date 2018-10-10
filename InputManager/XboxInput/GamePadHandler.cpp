#include "GamePadHandler.h"

std::unique_ptr<DirectX::GamePad> GamePadHandler::m_gamePad;
DirectX::GamePad::ButtonStateTracker GamePadHandler::m_buttons;
DirectX::GamePad::State GamePadHandler::m_state;

GamePadHandler::GamePadHandler()
{
	m_gamePad = nullptr;
}

GamePadHandler::~GamePadHandler()
{

}

GamePadHandler& GamePadHandler::Instance()
{
	static GamePadHandler inst;
	m_gamePad = std::make_unique<DirectX::GamePad>();
	m_gamePad->Resume();
	return inst;
}

void GamePadHandler::Resume()
{
	m_gamePad->Resume();
}

void GamePadHandler::Suspend()
{
	m_gamePad->Suspend();
}

bool GamePadHandler::UpdateState()
{
	m_state = m_gamePad->GetState(0);
	if (m_state.IsConnected())
	{
		m_buttons.Update(m_state);
		return true;
	}
	return false;
	
}

bool GamePadHandler::IsConnected()
{
	return m_state.IsConnected();
}

bool GamePadHandler::IsAPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.a == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	//m_state.thumbSticks.
}

bool GamePadHandler::IsBPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.b == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsYPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.y == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsXPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.x == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

float GamePadHandler::GetLeftStickYPosition()
{
	if(m_state.IsConnected())
	{
		return m_state.thumbSticks.leftY;
	}
	
	return 0.0f;
}

float GamePadHandler::GetLeftStickXPosition()
{
	if (m_state.IsConnected())
	{
		return m_state.thumbSticks.leftX;
	}

	return 0.0f;
}

float GamePadHandler::GetRightStickYPosition()
{
	if (m_state.IsConnected())
	{
		return m_state.thumbSticks.rightY;
	}

	return 0.0f;
}

float GamePadHandler::GetRightStickXPosition()
{
	if (m_state.IsConnected())
	{
		return m_state.thumbSticks.rightX;
	}

	return 0.0f;
}

DirectX::XMFLOAT2 GamePadHandler::GetLeftStickposition()
{
	if (m_state.IsConnected())
	{
		
		return DirectX::XMFLOAT2(m_state.thumbSticks.leftX, m_state.thumbSticks.leftY);
	}
	return DirectX::XMFLOAT2(0,0);
}

DirectX::XMFLOAT2 GamePadHandler::GetRightStickposition()
{
	if (m_state.IsConnected())
	{
		return DirectX::XMFLOAT2(m_state.thumbSticks.rightX, m_state.thumbSticks.rightY);
	}
	return DirectX::XMFLOAT2(0, 0);
}

bool GamePadHandler::IsLeftStickPressed()
{
	if (m_state.IsConnected())
	{
		return m_state.IsLeftStickPressed();
	}
}

bool GamePadHandler::IsRightTriggerPressed()
{
	if (m_state.IsConnected())
	{
		//return m_buttons.rightTrigger;
		if (m_buttons.rightTrigger == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool GamePadHandler::IsLeftTriggerPressed()
{
	if (m_state.IsConnected())
	{
		//return m_buttons.rightTrigger;
		if (m_buttons.leftTrigger == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool GamePadHandler::IsRightDpadPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.dpadRight == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsLeftDpadPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.dpadLeft == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsUpDpadPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.dpadUp == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsDownDpadPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.dpadDown == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsLeftShoulderPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.leftShoulder == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool GamePadHandler::IsRightShoulderPressed()
{
	if (m_state.IsConnected())
	{
		if (m_buttons.rightShoulder == DirectX::GamePad::ButtonStateTracker::PRESSED)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

