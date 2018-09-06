#include <Windows.h>
#include "Source/3D Engine/RenderingManager.h"
#include "Source/Shader/ShaderManager.h"

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

	RenderingManager renderingManager;

	renderingManager.Init(hInstance);



	while (renderingManager.getWindow().isOpen())
	{
		renderingManager.Update();
		renderingManager.Flush();
	}
	DX::g_shaderManager.Release();
	renderingManager.Release();
	return 0;
}