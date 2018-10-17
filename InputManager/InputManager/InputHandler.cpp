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

bool InputHandler::m_windowInFocus;
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

std::tuple<float, float> InputHandler::getMousePositionLHLUA()
{
	float y = (float)m_windowSize.y - m_mousePos.y;
	return std::tuple<float, float>(m_mousePos.x, y);
}

DirectX::XMINT2 InputHandler::getWindowSize()
{
	return m_windowSize; 
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

bool InputHandler::getWindowFocus()
{
	return m_windowInFocus;
}

std::tuple<int, int> InputHandler::getWindowSizeLUA()
{
	return std::tuple<int, int>(m_windowSize.x, m_windowSize.y);
}

void InputHandler::REGISTER_TO_LUA()
{
	static bool isRegged = false;

	if (!isRegged)
	{
		LUA::LuaTalker * m_talker = LUA::LuaTalker::GetInstance();
		sol::state_view * mSolStateView = m_talker->getSolState();

		mSolStateView->new_usertype<InputHandler>(LUA_INPUT,
			"new", sol::no_constructor, 
			LUA_INPUT_IS_KEY_PRESSED, &InputHandler::isKeyPressed,
			LUA_INPUT_IS_LEFT_MOUSE_PRESSED, &InputHandler::isMLeftPressed,
			LUA_INPUT_IS_MIDDLE_MOUSE_PRESSED, &InputHandler::isMMiddlePressed,
			LUA_INPUT_IS_RIGHT_MOUSE_PRESSED, &InputHandler::isMRightPressed,
			LUA_INPUT_GET_LAST_PRESSED, &InputHandler::getLastPressed,
			LUA_INPUT_GET_MOUSE_DELTA, &InputHandler::getMouseDelta,
			LUA_INPUT_GET_MOUSE_POS, &InputHandler::getMousePositionLUA,
			LUA_INPUT_GET_MOUSE_POS_LH, &InputHandler::getMousePositionLHLUA,
			LUA_INPUT_GET_WINDOW_SIZE, &InputHandler::getWindowSizeLUA
			
			);

		mSolStateView->new_enum(LUA_INPUT_KEY_ENUM,
			ENUM_TO_STRING(Del), Key::Del,
			ENUM_TO_STRING(Left), Key::Left,
			ENUM_TO_STRING(Up), Key::Up,
			ENUM_TO_STRING(Right), Key::Right,
			ENUM_TO_STRING(Down), Key::Down,
			ENUM_TO_STRING(Spacebar), Key::Spacebar,
			ENUM_TO_STRING(Comma), Key::Comma,
			ENUM_TO_STRING(Period), Key::Period,
			ENUM_TO_STRING(F5), Key::F5,
			ENUM_TO_STRING(F6), Key::F6,
			ENUM_TO_STRING(W), Key::W,
			ENUM_TO_STRING(A), Key::A,
			ENUM_TO_STRING(S), Key::S,
			ENUM_TO_STRING(D), Key::D,
			ENUM_TO_STRING(Shift), Key::Shift,
			ENUM_TO_STRING(Esc), Key::Esc,
			ENUM_TO_STRING(Backspace), Key::Backspace,
			ENUM_TO_STRING(Return), Key::Return
			//add more if we need more
			);

		isRegged = true;
	}
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

std::tuple<float, float> InputHandler::getMousePositionLUA()
{
	return std::tuple<float, float>(m_mousePos.x, m_mousePos.y);
}
