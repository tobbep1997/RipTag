#include "InputHandler.h"

bool InputHandler::m_keys[256];

bool InputHandler::m_mouseKeys[3];
bool InputHandler::m_mouseWasPressed[3];
DirectX::XMFLOAT2 InputHandler::m_mousePos;
float InputHandler::m_scrollDelta;

int InputHandler::m_lastPressed;
DirectX::XMINT2 InputHandler::m_windowSize;

DirectX::XMFLOAT2 InputHandler::m_mouseDelta;

DirectX::XMFLOAT2 InputHandler::m_windowPos;
DirectX::XMFLOAT2 InputHandler::m_viewportPos;
DirectX::XMINT2 InputHandler::m_viewportSize;

bool InputHandler::m_windowInFocus;

BOOL InputHandler::m_showCursor;
bool InputHandler::m_update;

bool InputHandler::m_closeGame;

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

//FUCKING READ THIS
//THIS IS NOT THE MOUSE POS DELTA. IT'S SCROLL DELTA
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

DirectX::XMINT2 InputHandler::getViewportSize()
{
	return m_viewportSize;
}

DirectX::XMFLOAT2 InputHandler::getMousePosDelta()
{
	/*DirectX::XMFLOAT2 temp = m_mouseDelta;
	m_mouseDelta.x =  0;
	m_mouseDelta.y =  0;
	return temp;*/

	float x =  m_mousePos.x - m_windowPos.x;
	float y =  m_mousePos.y - m_windowPos.y;
	return DirectX::XMFLOAT2(x, y);
	//return { x, y };
}

DirectX::XMFLOAT2 InputHandler::getWindowPos()
{
	return m_windowPos;
}

DirectX::XMFLOAT2 InputHandler::getviewportPos()
{
	return m_viewportPos;
}

bool InputHandler::getWindowFocus()
{
	return m_windowInFocus;
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
	m_update = false;
	m_closeGame = false;
}

InputHandler::~InputHandler()
{
}

InputHandler * InputHandler::Instance()
{
	static InputHandler instance;
	m_windowInFocus = true;
	return &instance; 
}

DirectX::XMFLOAT2 InputHandler::getMousePosition()
{
	return m_mousePos; 
}

void InputHandler::setShowCursor(BOOL b)
{
	m_showCursor = b;
	m_update = true;
	/*if (b) {
		while (ShowCursor(b) <= 0) {}
		
	}
	else {
		while (ShowCursor(b) >= 0) {}

	}*/
}

BOOL InputHandler::getShowCursor()
{
	return m_showCursor;
}

void InputHandler::WindowSetShowCursor()
{
	if (m_update)
	{

		if (m_showCursor) {
			while (ShowCursor(m_showCursor) <= 0) {}

		}
		else {
			while (ShowCursor(m_showCursor) >= 0) {}

		}
		m_update = false;
	}
	
}

void InputHandler::CloseGame()
{
	m_closeGame = true;
}

bool InputHandler::GetClosedGame()
{
	return m_closeGame;  
}
