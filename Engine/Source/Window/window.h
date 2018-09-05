#pragma once
#include <Windows.h>
#include <string>
#include "../Messages/CoreMessage.h"
#pragma warning(disable : 4838)

struct WindowContext
{
	UINT clientHeight;
	UINT clientWidth;
	bool fullscreen;

	LPCWSTR windowTitle;
	DWORD wndStyle;

	HINSTANCE windowInstance;
	WNDCLASSEX wcex;
};

class Window
{
private:
	HWND m_wHandler; 
	WindowContext m_windowContext; 
	MSG m_msg;

public:
	Window(); 
	~Window(); 

	bool Init(WindowContext windowContext); //Creating window and calls show
	void Update(CoreMessage msg); //Updates the window, based on incomming msg

	void PollEvents(); //Get the window msg like "destory" when X button pressed

	bool isOpen(); //Checks if the window is open

	WindowContext& getWindowContext(); //Get the windowcontext like "title", "width", "height"
	HWND& getHandler();


	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM); 
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); //This is where msg are executed

private:
	void _resize();
	void _setPosition();
	void _toggleFullScreen();
	void _destroy();
};