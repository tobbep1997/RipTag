#pragma once
//Make sure includes from the network lib are put at the top. This is to avoid 'rereference' error from Windows headers
#include <Multiplayer.h>


#include "EngineSource/3D Engine/RenderingManager.h"
#include <stack>
#include "States/State.h"
#include "States/PlayState.h"
#include "EngineSource/3D Engine/Model/Managers/ModelManager.h"
#include "States/MainMenu.h"
#include <LuaTalker.h>

class Game
{
private:
	RenderingManager * m_renderingManager = 0;
	Network::Multiplayer * pNetworkInstance = 0;

	std::stack<State*> m_gameStack;

	bool isPressed = false;
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
	//LUA EXPOSE
	void PushStateLUA(State * ptr);
	void PopStateLUA();
	static void REGISTER_TO_LUA(Game & gameInstance);

private:
	void _handleStateSwaps();

	void _restartGameIf();
};
