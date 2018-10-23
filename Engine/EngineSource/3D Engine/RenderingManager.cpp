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

RenderingManager * RenderingManager::GetInstance()
{
	static RenderingManager m_instance;
	return &m_instance;
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
	wind.fullscreen = true;
	wind.windowInstance = hInstance;
	wind.windowTitle = L"RipTag";
	//Will override the settings above
	//SettingLoader::LoadWindowSettings(wind);
	m_wnd.Init(wind);

	m_engine.Init(m_wnd.getHandler(), wind.fullscreen, wind.clientWidth, wind.clientHeight);

	if (DEBUG)
	{
		m_ImGuiManager.Init(m_wnd.getHandler());
	}
	
	
}

void RenderingManager::Update()
{
	m_wnd.PollEvents();
	if (DEBUG)
	{
		m_ImGuiManager.ImGuiProcPoll(m_wnd.getWindowProcMsg());
	}
#if _DEBUG
	if (GetAsyncKeyState(int('P')))
	{
		_reloadShaders();
	}
#endif
}

void RenderingManager::Clear()
{
	this->m_engine.Clear();
}

void RenderingManager::Flush(Camera & camera)
{
	//Draws Everything in the queue
	m_engine.Flush(camera);
	
	


	if(DEBUG)
	{
		m_ImGuiManager.Draw();
	}

	m_engine.Present();
	DX::g_deviceContext->ClearState();
}

void RenderingManager::Release()
{
	m_engine.Release();
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
	return m_wnd;
}

ProcMsg RenderingManager::getWindowProcMsg()
{
	return m_wnd.getWindowProcMsg();
}

void RenderingManager::_reloadShaders()
{
	//Unload the shader
	DX::g_shaderManager.ReloadAllShaders();
	/*DX::g_shaderManager.UnloadShader(L"../Engine/EngineSource/Shader/PixelShader.hlsl");
	DX::g_shaderManager.UnloadShader(L"../Engine/EngineSource/Shader/Shaders/ShadowPixel.hlsl");
	DX::g_shaderManager.UnloadShader(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl");

	//Reload shader
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowPixel.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl");
	*/
}
