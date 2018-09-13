#include "RenderingManager.h"
#include "../SettingsLoader/SettingsLoader.h"
#include "../Debugg//ImGui/imgui.h"
#include "../Debugg/ImGui/imgui_impl_win32.h"
#include "../Debugg/ImGui/imgui_impl_dx11.h"


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
	wind.clientWidth = 1280;
	wind.clientHeight = 720;
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
	if (GetAsyncKeyState(int('P')))
	{
		_ReloadShaders();
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
	DX::g_deviceContext->ClearState();
}

void RenderingManager::Release()
{
	engine.Release();
	if (DEBUG)
	{
		m_ImGuiManager.Release();
	}
}

void RenderingManager::ImGuiStartFrame()
{
	//Starts the new ImGui frame
	//TODO: This might be changed. This needs to be called before ImGui::begin()
	if (DEBUG) {
		m_ImGuiManager.StartFrame();
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

void RenderingManager::_ReloadShaders()
{
	//Unload the shader
	DX::g_shaderManager.UnloadShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	DX::g_shaderManager.UnloadShader(L"../Engine/Source/Shader/Shaders/ShadowPixel.hlsl");

	//Reload shader
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/Shaders/ShadowPixel.hlsl");
}
