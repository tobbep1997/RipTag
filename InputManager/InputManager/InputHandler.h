#pragma once 
#include <DirectXMath.h>
#include "EngineSource/Window/window.h"

class InputHandler
{
	friend class Window; 
private:

	static bool m_keys[256];
	static bool m_keysReleased[256];
	static bool m_keysPressed[256];
	
	static bool m_mouseKeys[3];
	static bool m_mouseWasPressed[3]; 
	static DirectX::XMFLOAT2 m_mousePos; 
	static float m_scrollDelta; 
	static DirectX::XMINT2 m_windowSize; 
	static int m_lastPressed; 

	static DirectX::XMFLOAT2 m_windowPos;
	static DirectX::XMFLOAT2 m_viewportPos;
	static DirectX::XMINT2 m_viewportSize;
	static DirectX::XMFLOAT2 m_mouseDelta;

	static bool m_windowInFocus;
	InputHandler();

	static BOOL m_showCursor;
	static bool m_update;

	static bool m_closeGame;

public:
	~InputHandler();

	static void Reset();

	//A singelton, only one instance
	static InputHandler * Instance(); 

	static DirectX::XMFLOAT2 getMousePosition(); 
	// This will spam through frames
	static bool isKeyPressed(int keyCode);
	// This is true one frame after the key is released
	static bool isKeyReleased(int keyCode);
	/* This is is one frame if the key were pressed,
	the key needs to be released for this to become true again. */
	static bool wasKeyPressed(int keyCode);
	static bool isMLeftPressed(bool repeat); 
	static bool isMMiddlePressed(); 
	static bool isMRightPressed(); 
	static int getLastPressed(); 
	static float getMouseDelta(); 
	static DirectX::XMFLOAT2 getMousePositionLH(); 
	static DirectX::XMINT2 getWindowSize(); 
	static DirectX::XMINT2 getViewportSize();
	static DirectX::XMFLOAT2 getMousePosDelta();
	static DirectX::XMFLOAT2 getWindowPos();
	static DirectX::XMFLOAT2 getviewportPos();

	static bool getWindowFocus();

	static void setShowCursor(BOOL b);
	static BOOL getShowCursor();

	static void WindowSetShowCursor();
	static void CloseGame();
	static bool GetClosedGame();
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
		Ctrl = 0x11,
		Alt = 0x12,
		Esc = 0x1B,
		Backspace = 0x08,
		Return = 0x0D,
		Enter = 0x0D
	};


};