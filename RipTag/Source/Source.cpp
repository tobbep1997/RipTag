#include "Game/Game.h"
#include "Timer/DeltaTime.h"
#pragma comment(linker, "/STACK:2000000")
#pragma comment(linker, "/HEAP:2000000")

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

	Game game;
	game.Init(hInstance);

	DeltaTime dt;
	float deltaTime = 0.0f;
	
	while (game.isRunning())
	{
		deltaTime = dt.getDeltaTimeInSeconds();

		game.Clear();
		game.PollEvents();
		game.ImGuiFrameStart();
		game.Update(deltaTime);
		game.Draw();
	}

	DX::g_shaderManager.Release();
	return 0;
	

}