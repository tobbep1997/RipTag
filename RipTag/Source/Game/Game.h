#pragma once
#include <stack>

class RenderingManager;
class PlayState;
class State;

namespace Network
{
	class Multiplayer;
}

class Game
{
private:
	bool m_justSwaped = false;
	RenderingManager * m_renderingManager = 0;
	Network::Multiplayer * pNetworkInstance = 0;

	std::stack<State*> m_gameStack;

	bool isPressed = false;
public:
	Game();
	~Game();

	//Init
	void Init(_In_ HINSTANCE hInstance, bool dbg = false);
	//------------------------------------
	//VitalFunctions to make the game work
	bool isRunning();			//Is game running
	void PollEvents();			//Window Events
	void PollSingelThread();
	void Clear();				//Clear screen
	void Update(double deltaTime);				//Updates everything, ex: setPosition();
	void Draw();				//Calls Draw
	void ImGuiFrameStart();		//ImGuiStart
	//------------------------------------

	void ImGuiPoll();

private:
	void _handleStateSwaps();

	void _restartGameIf();
};
