#include <chrono>
#include "Game/Game.h"
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

	const float REFRESH_RATE = 60.0f;

	
	using namespace std::chrono;
	auto time = steady_clock::now();
	auto timer = steady_clock::now();
	int updates = 0;
	int fpsCounter = 0;
	float freq = 1000000000.0f / REFRESH_RATE;
	float unprocessed = 0;


	while (game.isRunning())
	{
		//HEAVY SHIT
		game.Clear();
		game.PollEvents();

		auto currentTime = steady_clock::now();
		auto dt = duration_cast<nanoseconds>(currentTime - time).count();
		time = steady_clock::now();

		unprocessed += (dt / freq);

		while (unprocessed > 1)
		{
			updates++;
			unprocessed -= 1;
			game.Update();
		}


		game.Draw();

		if (duration_cast<milliseconds>(steady_clock::now() - timer).count() > 1000)
		{
			updates = 0;
			fpsCounter = 0;
			timer += milliseconds(1000);
		}

	}


	return 0;
	

}