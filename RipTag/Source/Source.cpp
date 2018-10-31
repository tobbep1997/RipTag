#include "Game/Game.h"
#include "Timer/DeltaTime.h"
#include "EngineSource/Helper/Timer.h"
#include <LuaTalker.h>
#include <AudioEngine.h>

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
	AudioEngine::Init();
	
	Game game;
	game.Init(hInstance);
	std::cout << "hello";
	DeltaTime dt;
	float deltaTime = 0.0f;
	float deltaNega = 0;



	while (game.isRunning())
	{

		deltaTime = dt.getDeltaTimeInSeconds();
		if (deltaTime > 1.0f)
			deltaTime = 1 / 60.0f;

		//This is to avoid Pollevents from fucking with the game
		deltaTime = deltaTime - deltaNega;
		game.Clear();

		//Pollevents
		Timer::StartTimer();
		game.PollEvents();
		Timer::StopTimer();
		deltaNega = Timer::GetDurationInSeconds();
		//Draw and update
		game.ImGuiFrameStart();
		game.Update(deltaTime);
		AudioEngine::Update();
		game.Draw();
	}

	DX::g_shaderManager.Release();
	AudioEngine::Release();
	return 0;
	

}