#pragma once
#include <Windows.h>
#include "Structs.h"
class Window
{
private:
	HWND m_wHandler; 
	window::WindowContext m_windowContext; 
	void _resize(); 
	void _setPosition(); 
	void _toggleFullScreen(); 
	void _destroy(); 

public:
	Window(); 
	~Window(); 
	bool Init(window::WindowContext windowContext); 
	void Update(); //Pass through message vector here. 
	window::WindowContext& getWindowContext();
	HWND& getHandler();

	virtual LRESULT msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


};