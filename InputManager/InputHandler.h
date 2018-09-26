#pragma once 
#include <DirectXMath.h>
#include "../Engine/Source/Window/window.h"
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

	InputHandler();
	~InputHandler();

public:

	//A singelton, only one instance
	static InputHandler& Instance(); 

	static DirectX::XMFLOAT2 getMousePosition(); 
	static bool isKeyPressed(int keyCode);
	static bool isMLeftPressed(bool repeat); 
	static bool isMMiddlePressed(); 
	static bool isMRightPressed(); 
	static int getLastPressed(); 
	static float getMouseDelta(); 
	static DirectX::XMFLOAT2 getMousePositionLH(); 
	static DirectX::XMINT2 getWindowSize(); 


	enum Key
	{
		Del = 46,
		LeftArrow = 37,
		UpArrow = 38,
		RightArrow = 39,
		DownArrow = 40,
		SPACEBAR = 0x20,
		Comma = 0xBC,
		Period = 0xBE,
		F5 = 0x74,
		F6 = 0x75,
		WKey = 0x57,
		AKey = 0x41,
		SKey = 0x53,
		DKey = 0x44,
		Shift = 0x10,
		Esc = 0x1B,
		BackSpace = 0x08
	};


};