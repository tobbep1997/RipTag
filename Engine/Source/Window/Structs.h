#pragma once
#include <Windows.h>
#include <string>

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
