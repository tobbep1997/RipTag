#pragma once
#include <Windows.h>
#include <string>
namespace window
{
	struct WindowContext
	{
		UINT posX; 
		UINT posY; 
		UINT clientHeight; 
		UINT clientWidth;
		bool fullscreen;

		std::string windowTitle; 
		DWORD wndStyle; 
		
		HINSTANCE windowInstance; 
		WNDCLASSEX wcex;
	}; 
}