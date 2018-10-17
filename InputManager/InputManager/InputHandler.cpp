#include "InputHandler.h"

bool InputHandler::m_keys[256];

bool InputHandler::m_mouseKeys[3];
bool InputHandler::m_mouseWasPressed[3];
DirectX::XMFLOAT2 InputHandler::m_mousePos;
float InputHandler::m_scrollDelta;

int InputHandler::m_lastPressed;
DirectX::XMINT2 InputHandler::m_windowSize;

bool InputHandler::isKeyPressed(int keyCode)
{
	return m_keys[keyCode]; 
}

bool InputHandler::isMLeftPressed(bool repeat)
{
	//Hold
	if (repeat)
		return m_mouseKeys[0];

	//Click
	bool result = false;

	bool pressed = m_mouseKeys[0];
	if (pressed && !m_mouseWasPressed[0])
	{
		result = true; 
	}
	m_mouseWasPressed[0] = pressed;

	return result; 
}

bool InputHandler::isMMiddlePressed()
{
	return m_mouseKeys[1];
}

bool InputHandler::isMRightPressed()
{
	return m_mouseKeys[2]; 
}

int InputHandler::getLastPressed()
{
	return m_lastPressed; 
}

float InputHandler::getMouseDelta()
{
	float returnDelta = m_scrollDelta; 
	m_scrollDelta = 0.0f; 
	return returnDelta; 
}

DirectX::XMFLOAT2 InputHandler::getMousePositionLH()
{
	DirectX::XMFLOAT2 position; 

	position.x = m_mousePos.x; 
	position.y = static_cast<float>(m_windowSize.y) - m_mousePos.y; 

	return position; 
}

DirectX::XMINT2 InputHandler::getWindowSize()
{
	return m_windowSize; 
}

InputHandler::InputHandler()
{
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false; 
	}

	for (int i = 0; i < 3; i++)
	{
		m_mouseKeys[i] = false; 
		m_mouseWasPressed[i] = false; 
	}
}

InputHandler::~InputHandler()
{
}

InputHandler * InputHandler::Instance()
{
	static InputHandler instance; 
	return &instance; 
}

DirectX::XMFLOAT2 InputHandler::getMousePosition()
{
	return m_mousePos; 
}