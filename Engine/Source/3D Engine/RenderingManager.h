#pragma once
#include "../Window/window.h"
#include "Engine3D.h"
class RenderingManager
{
private:
	Window wnd;
	Engine3D engine;
public:
	RenderingManager();
	~RenderingManager();

	void Init(HINSTANCE hInstance);
	void Update();

	void Release();

	Window& GetWindow();
};

