#include "Game/Game.h"
#include "Timer/DeltaTime.h"

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
		if (deltaTime > 1.0f)
			deltaTime = 1 / 60.0f;

		game.Clear();
		game.PollEvents();
		game.ImGuiFrameStart();
		game.Update(deltaTime);
		game.Draw();
	}

	DX::g_shaderManager.Release();
	return 0;
	

}