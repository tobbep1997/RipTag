#pragma once
#include <Windows.h>
#include <string>
#include "../Messages/CoreMessage.h"
#include "../InputManager/InputHandler.h"
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

struct ProcMsg
{
	HWND hwnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
};

class Window
{
private:
	HWND m_wHandler; 
	WindowContext m_windowContext; 
	MSG m_Peekmsg;

	ProcMsg m_procMsg;

public:
	Window(); 
	~Window(); 

	bool Init(WindowContext windowContext); //Creating window and calls show
	void Update(CoreMessage msg); //Updates the window, based on incomming msg

	//Get the window msg like "destory" when X button pressed
	void PollEvents(); 
	//Checks if the window is open
	bool isOpen(); 
	void Close();

	WindowContext& getWindowContext(); //Get the windowcontext like "title", "width", "height"
	HWND& getHandler();

	
	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM); 
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); //This is where msg are executed

	ProcMsg & getWindowProcMsg(); //Returns the proc msg from the window
private:
	void _resize(UINT width, UINT height);	//Will Resize the window to the params specified
	void _setPosition(UINT posX, UINT posY); //Will Set the position of the window
	void _toggleFullScreen();	
	void _destroy();
};