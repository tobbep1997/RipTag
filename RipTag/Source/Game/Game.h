#pragma once
#include "Source/3D Engine/RenderingManager.h"
#include <stack>
#include "States/State.h"
#include "States/PlayState.h"
#include "Source/3D Engine/Model/ModelManager.h"

class Game
{
private:
	RenderingManager m_renderingManager;

	std::stack<State*> m_gameStack;
	ModelManager modelManager;

public:
	Game();
	~Game();
	void Init(_In_ HINSTANCE hInstance);

	bool isRunning();

	void PollEvents();

	void Clear();
	
	void Update();

	void Draw();

	void ImGuiFrameStart();
private:

};
