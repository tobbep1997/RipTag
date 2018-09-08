#include "RenderingManager.h"
#include "../SettingsLoader/SettingsLoader.h"
#include "../Debug/ImGui/imgui.h"
#include "../Debug/ImGui/imgui_impl_win32.h"
#include "../Debug/ImGui/imgui_impl_dx11.h"


RenderingManager::RenderingManager()
{
	
}


RenderingManager::~RenderingManager()
{
	
}

void RenderingManager::Init(HINSTANCE hInstance)
{
#if _DEBUG
	DEBUG = true;
#else
	DEBUG = false;
#endif

	WindowContext wind;
	wind.clientHeight = 1000;
	wind.clientWidth = 1000;
	wind.fullscreen = false;
	wind.windowInstance = hInstance;
	wind.windowTitle = L"Victor �r Gay";
	//Will override the settings above
	//SettingLoader::LoadWindowSettings(wind);
	wnd.Init(wind);

	engine.Init(wnd.getHandler(), wind.fullscreen, wind.clientWidth, wind.clientHeight);

	if (DEBUG)
	{
		m_ImGuiManager.Init(wnd.getHandler());
	}
	
	
}

void RenderingManager::Update()
{
	wnd.PollEvents();
	if (DEBUG)
	{
		m_ImGuiManager.ImGuiProcPoll(wnd.getWindowProcMsg());
	}
}

void RenderingManager::Flush(Camera & camera)
{
	//Draws Everything in the queue
	engine.Flush(camera);

	if(DEBUG)
	{
		m_ImGuiManager.Draw();
	}

	engine.Present();
	
}

void RenderingManager::Release()
{
	engine.Release();
	if (DEBUG)
	{
		m_ImGuiManager.Release();
	}
}

Window& RenderingManager::getWindow()
{
	return wnd;
}

ProcMsg RenderingManager::getWindowProcMsg()
{
	return wnd.getWindowProcMsg();
}
