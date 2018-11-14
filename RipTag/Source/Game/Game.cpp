#include "RipTagPCH.h"
#include "Game.h"
#include <filesystem>

Game::Game()
{
	
}

Game::~Game()
{
	m_renderingManager->Release();
	pNetworkInstance->Destroy();
	while(!m_gameStack.empty())
	{
		delete m_gameStack.top();
		m_gameStack.pop();
	}
}

void Game::Init(_In_ HINSTANCE hInstance)
{
	//Rendering Manager Start
	{
		m_renderingManager = RenderingManager::GetInstance();
		m_renderingManager->Init(hInstance);
	}

	//Input handler and mapping
	{
		GamePadHandler::Instance();
		Input::Instance();
		//Reading player settings file
		Input::ReadSettingsFile();
		//Input::WriteSettingsToFile();
		//----------------------------
		InputMapping::Init();
	}
	Timer::Instance();

	//Network Start
	{
		pNetworkInstance = Network::Multiplayer::GetInstance();
		pNetworkInstance->Init();
	}

	m_gameStack.push(new MainMenu(m_renderingManager));
	m_gameStack.top()->Load();
}

bool Game::isRunning()
{
	return m_renderingManager->getWindow().isOpen();
}

void Game::PollEvents()
{
	m_renderingManager->Update();

}

void Game::PollSingelThread()
{
	m_renderingManager->UpdateSingleThread();
}

void Game::Clear()
{
	//TODO Fix clear
	m_renderingManager->Clear();
}

void Game::Update(double deltaTime)
{
#if _DEBUG
	_restartGameIf();
#endif

	_handleStateSwaps();
	GamePadHandler::UpdateState();
	m_gameStack.top()->Update(deltaTime);
	InputMapping::Call();
	pNetworkInstance->Update();

	InputHandler::getRawInput();
}

void Game::Draw()
{
	m_gameStack.top()->Draw();
}

void Game::ImGuiFrameStart()
{
	m_renderingManager->ImGuiStartFrame();
}

void Game::ImGuiPoll()
{
	m_renderingManager->ImGuiProc();
}

void Game::_handleStateSwaps()
{
	if (m_gameStack.top()->getNewState() != nullptr)
	{
		m_gameStack.top()->unLoad();
		m_gameStack.push(m_gameStack.top()->getNewState());
		m_gameStack.top()->Load();
	}

	if (m_gameStack.top()->getKillState())
	{
		m_gameStack.top()->unLoad();
		delete m_gameStack.top();
		m_gameStack.pop();
		m_gameStack.top()->pushNewState(nullptr);
		m_gameStack.top()->Load();
	}

	if (m_gameStack.top()->getBackToMenu())
	{
		while (m_gameStack.size() > 1)
		{
			m_gameStack.top()->unLoad();
			delete m_gameStack.top();
			m_gameStack.pop();
			m_gameStack.top()->pushNewState(nullptr);
		}
		m_gameStack.top()->Load();
	}
}

void Game::_restartGameIf()
{
	/*if (InputHandler::isKeyPressed('B'))
	{
		if (isPressed == false)
		{
			delete m_gameStack.top();
			m_gameStack.pop();

			m_gameStack.push(new PlayState(m_renderingManager));
			isPressed = true;
		}
	}
	else
	{
		isPressed = false;
	}*/
}
