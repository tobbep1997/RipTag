#include "RipTagPCH.h"
#include "Game/Game.h"
#include "EngineSource/Helper/Timer.h"
#include <AudioEngine.h>

#include "EngineSource/Shader/ShaderManager.h"

#if _DEBUG
//Allocates memory to the console
void _alocConsole() {
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
}
#endif

void GameLoop(Game * game)
{
	DeltaTime dt;
	float deltaTime = 0.0f;
	float deltaNega = 0;
	while (game->isRunning())
	{

		deltaTime = dt.getDeltaTimeInSeconds();
		if (deltaTime > 1.0f)
			deltaTime = 1 / 60.0f;

		//This is to avoid Pollevents from fucking with the game
		game->Clear();

		//Pollevents
		

		//Draw and update
		game->ImGuiFrameStart();
		game->Update(deltaTime);
		AudioEngine::Update();
		game->Draw();
	}
}

void SingleGameLoop(Game * game)
{
	DeltaTime dt;
	float deltaTime = 0.0f;
	float deltaNega = 0;
	while (game->isRunning())
	{
		InputHandler::Reset();
		deltaTime = dt.getDeltaTimeInSeconds();
		if (deltaTime > 1.0f)
			deltaTime = 1 / 60.0f;
		game->PollSingelThread();

		//This is to avoid Pollevents from fucking with the game
		game->Clear();

		///-------------------



		///-------------------

		//Pollevents

		//Draw and update
		game->ImGuiFrameStart();
		game->Update(deltaTime);
		AudioEngine::Update();
		game->Draw();
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if _DEBUG
	_alocConsole();
#endif
    AudioEngine::Init();
	

	Game game;
	game.Init(hInstance);
	std::cout << "hello";

	/*std::thread gameLoop;
	gameLoop = std::thread(&GameLoop, &game);
	game.PollEvents();

	//gameLoop.join();*/

	SingleGameLoop(&game);
	
	/*Grid grid = Grid(300, 300);
	std::vector<Node*> path;
	
	dt.getDeltaTimeInSeconds();

	std::cout << "Printing path..." << std::endl << std::endl;
	for (int i = 0; i < path.size(); i++)
	{
		std::cout << "x: " << path.at(i)->tile.getX() << " y: " << path.at(i)->tile.getY() << std::endl;
	}
	std::cout << std::endl << "Path is finished printing..." << std::endl;

	system("pause");

	for (int i = 0; i < path.size(); i++)
	{
		delete path.at(i);
		path.at(i) = nullptr;
	}*/

	

	DX::g_shaderManager.Release();
	AudioEngine::Release();
	return 0;
	

}