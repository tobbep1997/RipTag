#pragma once
#include "Source/3D Engine/RenderingManager.h"
#include <stack>
#include "States/State.h"

class Game
{
private:
	RenderingManager m_renderingManager;

	std::stack<State> m_gameStack;

public:
	Game();
	~Game();
	void Init(HINSTANCE hInstance);

	bool isRunning();

	void PollEvents();

	void Clear();
	
	void Update();

	void Draw();
private:

};
