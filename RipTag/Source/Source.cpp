

#include "Game/Game.h"
#include "Timer/DeltaTime.h"
#include "EngineSource/Helper/Timer.h"
#include <LuaTalker.h>
#include "../RipTag/Source/Game/Pathfinding/Grid.h"





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
	std::cout << "hello";
	DeltaTime dt;
	float deltaTime = 0.0f;
	float deltaNega = 0;

	Grid grid = Grid(300, 300);
	std::vector<Node*> path;
	//path = grid.FindPath(Tile(0, 0), Tile(299, 299));
	bool nP = false;
	dt.getDeltaTimeInSeconds();
	grid.ThreadPath(Tile(0, 0), Tile(299, 299));

	while (game.isRunning())
	{
		if (!nP)
		{
			if (grid.ready())
			{
				path = grid.getPath();
				std::cout << dt.getDeltaTimeInSeconds() << " seconds." << std::endl;
				std::cout << "Printing path..." << std::endl << std::endl;
				for (int i = 0; i < path.size(); i++)
				{
					std::cout << "x: " << path.at(i)->tile.getX() << " y: " << path.at(i)->tile.getY() << std::endl;
				}
				std::cout << std::endl << "Path is finished printing..." << std::endl;
				for (int i = 0; i < path.size(); i++)
				{
					delete path.at(i);
					path.at(i) = nullptr;
				}
				nP = true;
			}
		}
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
		game.Draw();
	}

	DX::g_shaderManager.Release();
	return 0;
	

}