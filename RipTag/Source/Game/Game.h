#pragma once
#include "Source/3D Engine/RenderingManager.h"
#include <stack>
#include "States/State.h"
#include "States/PlayState.h"
#include "Source/3D Engine/Model/Managers/ModelManager.h"
#include "States/MainMenu.h"

class Game
{
private:
	RenderingManager m_renderingManager;

	std::stack<State*> m_gameStack;
	//TODO:: This is super temporary, beacuse the render needs 1 object to work
	ModelManager modelManager;
		
public:
	Game();
	~Game();

	//Init
	void Init(_In_ HINSTANCE hInstance);
	//------------------------------------
	//VitalFunctions to make the game work
	bool isRunning();			//Is game running
	void PollEvents();			//Window Events
	void Clear();				//Clear screen
	void Update(double deltaTime);				//Updates everything, ex: setPosition();
	void Draw();				//Calls Draw
	void ImGuiFrameStart();		//ImGuiStart
	//------------------------------------

private:
	void _handleStateSwaps();
};
