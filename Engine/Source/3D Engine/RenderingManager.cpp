#include "RenderingManager.h"
#include "../SettingsLoader/SettingsLoader.h"


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
	//Will override the settings above
	SettingLoader::LoadWindowSettings(wind);
	wnd.Init(wind);

	engine.Init(wnd.getHandler(), wind.fullscreen, wind.clientWidth, wind.clientHeight);
}

void RenderingManager::Update()
{
	wnd.PollEvents();
}

void RenderingManager::Flush()
{
	engine.Draw();
}

void RenderingManager::Release()
{
	engine.Release();
}

Window& RenderingManager::getWindow()
{
	return wnd;
}

ProcMsg RenderingManager::getWindowProcMsg()
{
	return wnd.getWindowProcMsg();
}
