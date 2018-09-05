#include <Windows.h>
#include "Source/Window/window.h"


#if _DEBUG
#include <iostream>
//Allocates memory to the console
void _alocConsole() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
}
#endif

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
	_alocConsole();
#endif

	Window wnd;
	WindowContext wind;
	wind.clientHeight = 500;
	wind.clientWidth = 500;
	wind.fullscreen = false;
	wind.windowInstance = hInstance;
	wind.windowTitle = L"gay";
	
	wnd.Init(wind);

	while (wnd.isOpen())
	{
		wnd.PollEvents();
		//std::cout << "hello world" << std::endl;
	}

	
	return 0;
}