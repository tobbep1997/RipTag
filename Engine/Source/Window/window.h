#pragma once
#include <Windows.h>
#include "Structs.h"
class Window
{
private:
	HWND m_wHandler; 
	WindowContext m_windowContext; 
	void _resize(); 
	void _setPosition(); 
	void _toggleFullScreen(); 
	void _destroy(); 

	MSG m_msg;
public:
	Window(); 
	~Window(); 
	bool Init(WindowContext windowContext); 
	void Update(); //Pass through message vector here. 

	void PollEvents();
	bool isOpen();

	WindowContext& getWindowContext();
	HWND& getHandler();


	static LRESULT CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


};