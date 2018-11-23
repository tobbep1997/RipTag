#include "EnginePCH.h"
#include "RenderingManager.h"


RenderingManager::RenderingManager()
{
	m_wnd = new Window();
	m_engine = new Engine3D();
	m_ImGuiManager = new ImGuiManager();
	m_wind = new WindowContext();
}


RenderingManager::~RenderingManager()
{
	delete m_wnd;
	delete m_engine;
	delete m_ImGuiManager;
	delete m_wind;
}

RenderingManager * RenderingManager::GetInstance()
{
	static RenderingManager m_instance;
	return &m_instance;
}

void RenderingManager::Init(HINSTANCE hInstance)
{
#if _DEBUG || _RELEASE_DBG
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
	m_wnd->Init(*m_wind);

	m_engine->Init(m_wnd->getHandler(), *m_wind);

	if (DEBUG)
	{
		m_ImGuiManager->Init(m_wnd->getHandler());
	}
	
	
}

void RenderingManager::Update()
{
	while (m_wnd->isOpen())
	{
		InputHandler::WindowSetShowCursor();
		m_wnd->PollEvents();
		if (DEBUG)
		{
			m_ImGuiManager->ImGuiProcPoll(m_wnd->getWindowProcMsg());
		}
	#if _DEBUG || _RELEASE_DBG
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
	if (DEBUG)
	{
		m_ImGuiManager->ImGuiProcPoll(m_wnd->getWindowProcMsg());
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
	this->m_engine->Clear();
}

void RenderingManager::Flush(Camera & camera)
{
	//Draws Everything in the queue
	m_engine->Flush(camera);
	
	if(DEBUG)
	{
		m_ImGuiManager->Draw();
	}

	m_engine->Present();
	DX::g_deviceContext->ClearState();
}

void RenderingManager::Release()
{
	m_engine->Release();
	if (DEBUG)
	{
		m_ImGuiManager->Release();
	}
}

void RenderingManager::ImGuiStartFrame()
{
	//Starts the new ImGui frame
	//TODO: This might be changed. This needs to be called before ImGui::begin()
	if (DEBUG) {
		m_ImGuiManager->StartFrame();
	}
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
	if (DEBUG)
	{
		m_ImGuiManager->ImGuiProcPoll(m_wnd->getWindowProcMsg());
	}
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

void RenderingManager::_reloadShaders()
{
	bool f = true;
	if (f)
		DX::g_shaderManager.ReloadAllShaders();
}
