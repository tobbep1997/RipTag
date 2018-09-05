#include "RenderingManager.h"



RenderingManager::RenderingManager()
{
	
}


RenderingManager::~RenderingManager()
{
	
}

void RenderingManager::Init(HINSTANCE hInstance)
{
	WindowContext wind;
	wind.clientHeight = 500;
	wind.clientWidth = 500;
	wind.fullscreen = false;
	wind.windowInstance = hInstance;
	wind.windowTitle = L"Victor är Gay";

	wnd.Init(wind);

	engine.Init(wnd.getHandler(), wind.fullscreen);
}

void RenderingManager::Update()
{
	wnd.PollEvents();
}

void RenderingManager::Release()
{
	engine.Release();
}

Window& RenderingManager::GetWindow()
{
	return wnd;
}
