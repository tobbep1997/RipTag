#include "EnginePCH.h"
#include "RenderingManager.h"


RenderingManager::RenderingManager()
{
	m_wnd = new Window();
	m_engine = new Engine3D();
	m_wind = new WindowContext();
}


RenderingManager::~RenderingManager()
{
	delete m_wnd;
	delete m_engine;
	delete m_wind;
}

RenderingManager * RenderingManager::GetInstance()
{
	static RenderingManager m_instance;
	return &m_instance;
}

void RenderingManager::Init(HINSTANCE hInstance)
{
#ifndef _DEPLOY
	DEBUG = true;
#else
	DEBUG = false;
#endif
	m_hInstance = hInstance;
	
	m_wind->clientWidth = 1280;
	m_wind->clientHeight = 720;
	m_wind->fullscreen = false;
	m_wind->windowInstance = hInstance;
	m_wind->windowTitle = L"RipTag";
	//Will override the settings above
	SettingLoader::LoadWindowSettings(*m_wind);
	SettingLoader::g_windowContext = m_wind;

	DX::g_backBufferResolution.bottom = 0;
	DX::g_backBufferResolution.left		  = 0;
	DX::g_backBufferResolution.right	  = m_wind->clientWidth;
	DX::g_backBufferResolution.top		  = m_wind->clientHeight;

	m_wnd->Init(*m_wind);

	m_engine->Init(m_wnd->getHandler(), *m_wind);


	
	
}

void RenderingManager::Update()
{
	while (m_wnd->isOpen())
	{
		InputHandler::WindowSetShowCursor();
		m_wnd->PollEvents();

	#ifndef _DEPLOY
		if (GetAsyncKeyState(int('P')))
		{
			_reloadShaders();
		}
	#endif

	}
}

void RenderingManager::UpdateSingleThread()
{
	InputHandler::WindowSetShowCursor();
	m_wnd->PollEvents();
#ifndef _DEPLOY
	if (GetAsyncKeyState(int('P')))
	{
		_reloadShaders();
	}
#endif
}

void RenderingManager::Clear()
{
	this->m_engine->Clear();
}

void RenderingManager::Flush(Camera & camera)
{
	//Draws Everything in the queue
	m_engine->Flush(camera);
	
	m_engine->Present();
	//DX::g_deviceContext->ClearState();
}

void RenderingManager::Release()
{
	m_engine->Release();
}



Window& RenderingManager::getWindow()
{
	return *m_wnd;
}

ProcMsg RenderingManager::getWindowProcMsg()
{
	return m_wnd->getWindowProcMsg();
}

void RenderingManager::ImGuiProc()
{

}

void RenderingManager::Reset()
{
	// TODO :: Reload window and engine

	//WindowContext wind;
	//wind.clientWidth = 1280;
	//wind.clientHeight = 720;
	//wind.fullscreen = false;
	//wind.windowInstance = m_hInstance;
	//wind.windowTitle = L"RipTag";
	////Will override the settings above
	//SettingLoader::LoadWindowSettings(wind);

	//m_engine->Release();
	//m_wnd->Init(wind);
	//m_engine->Init(m_wnd->getHandler(), wind.fullscreen, wind.clientWidth, wind.clientHeight);
	//_reloadShaders();

	//if (DEBUG)
	//{
	//	m_ImGuiManager->Release();
	//	m_ImGuiManager->Init(m_wnd->getHandler());
	//}
}
#pragma optimize("", off)
void RenderingManager::_reloadShaders()
{
	DX::g_shaderManager.ReloadAllShaders();
}
#pragma optimize("", on)

