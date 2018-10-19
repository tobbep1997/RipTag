#include "Game/Game.h"
#include "Timer/DeltaTime.h"
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

	DeltaTime dt;
	float deltaTime = 0.0f;
	
	Grid grid = Grid(30, 30);
	std::vector<Node*> path;

	grid.blockGrid();
	grid.printGrid();

	path = grid.FindPath(Tile(3, 10), Tile(13, 29));

	std::cout << "Printing path..." << std::endl << std::endl;
	for (int i = 0; i < path.size(); i++)
	{
		std::cout << "x: " << path.at(i)->tile.getX() << " y: " << path.at(i)->tile.getY() << std::endl;
	}
	std::cout << std::endl << "Path is finished printing..." << std::endl;

	grid.printGrid();
	system("pause");

	for (int i = 0; i < path.size(); i++)
	{
		delete path.at(i);
		path.at(i) = nullptr;
	}

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