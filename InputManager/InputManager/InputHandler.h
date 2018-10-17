#pragma once 
#include <DirectXMath.h>
#include <LuaTalker.h>
#include "EngineSource/Window/window.h"

#define LUA_INPUT "Input"
#define LUA_INPUT_IS_KEY_PRESSED "IsKeyPressed"
#define LUA_INPUT_IS_LEFT_MOUSE_PRESSED "IsLeftMousePressed"
#define LUA_INPUT_IS_MIDDLE_MOUSE_PRESSED "IsMiddleMousePressed"
#define LUA_INPUT_IS_RIGHT_MOUSE_PRESSED "IsRightMousePressed"
#define LUA_INPUT_GET_LAST_PRESSED "GetLastPressed"
#define LUA_INPUT_GET_MOUSE_DELTA "GetMouseDelta"
#define LUA_INPUT_GET_MOUSE_POS "GetMousePos"
#define LUA_INPUT_GET_MOUSE_POS_LH "GetMousePosLH"
#define LUA_INPUT_GET_WINDOW_SIZE "GetWindowSize"

#define LUA_INPUT_KEY_ENUM "Keys"

#define ENUM_TO_STRING(ENUM) std::string(#ENUM)

class InputHandler
{
	friend class Window; 
private:

	static bool m_keys[256];
	static bool m_mouseKeys[3];
	static bool m_mouseWasPressed[3]; 
	static DirectX::XMFLOAT2 m_mousePos; 
	static float m_scrollDelta; 
	static DirectX::XMINT2 m_windowSize; 
	static int m_lastPressed; 

	static DirectX::XMFLOAT2 m_windowPos;

	static DirectX::XMFLOAT2 m_mouseDelta;

	static bool m_windowInFocus;
	InputHandler();

public:
	~InputHandler();

	//A singelton, only one instance
	static InputHandler * Instance(); 

	static DirectX::XMFLOAT2 getMousePosition(); 
	static bool isKeyPressed(int keyCode);
	static bool isMLeftPressed(bool repeat); 
	static bool isMMiddlePressed(); 
	static bool isMRightPressed(); 
	static int getLastPressed(); 
	static float getMouseDelta(); 
	static DirectX::XMFLOAT2 getMousePositionLH(); 
	static DirectX::XMINT2 getWindowSize(); 

	static DirectX::XMFLOAT2 getMousePosDelta();
	static DirectX::XMFLOAT2 getWindowPos();

	static bool getWindowFocus();
	static std::tuple<float, float> getMousePositionLUA();
	static std::tuple<float, float> getMousePositionLHLUA();
	static std::tuple<int, int> getWindowSizeLUA();

	static void REGISTER_TO_LUA();
	enum Key
	{
		Del = 46,
		Left = 37,
		Up = 38,
		Right = 39,
		Down = 40,
		Spacebar = 0x20,
		Comma = 0xBC,
		Period = 0xBE,
		F5 = 0x74,
		F6 = 0x75,
		W = 0x57,
		A = 0x41,
		S = 0x53,
		D = 0x44,
		Shift = 0x10,
		Esc = 0x1B,
		Backspace = 0x08,
		Return = 0x0D
	};


};