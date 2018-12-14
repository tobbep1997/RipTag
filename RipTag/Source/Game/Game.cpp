#include "RipTagPCH.h"
#include "Game.h"
#include <filesystem>

Game::Game()
{
	
}

Game::~Game()
{
	while(!m_gameStack.empty())
	{
		m_gameStack.top()->unLoad();
		delete m_gameStack.top();
		m_gameStack.pop();
	}
	m_renderingManager->Release();
	pNetworkInstance->Destroy();
}

void Game::Init(_In_ HINSTANCE hInstance, bool dbg)
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

	if (dbg)
		m_gameStack.push(new DBGState(m_renderingManager));
	else
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
	if (!m_justSwaped)
	{
		
		GamePadHandler::UpdateState();
		m_gameStack.top()->Update(deltaTime);
		pNetworkInstance->Update();
	}
	else
		m_justSwaped = false;


	InputHandler::getRawInput();
}

void Game::Draw()
{
	m_gameStack.top()->Draw();
}

void Game::ImGuiPoll()
{
	m_renderingManager->ImGuiProc();
}

void Game::_handleStateSwaps()
{
	using namespace std::chrono_literals;
	if (m_gameStack.top()->getNewState() != nullptr)
	{
		m_gameStack.top()->unLoad();
		DX::g_deviceContext->ClearState();
		//std::this_thread::sleep_for(5s);
		m_gameStack.push(m_gameStack.top()->getNewState());
		m_gameStack.top()->Load();
		m_justSwaped = true;
	}
	if (m_gameStack.top()->GetReset() != nullptr)
	{
		State * ss = m_gameStack.top()->GetReset();
		m_gameStack.top()->resetState(nullptr);
		m_gameStack.top()->unLoad();
		delete m_gameStack.top();
		DX::g_deviceContext->ClearState();
		//std::this_thread::sleep_for(5s);
		m_gameStack.pop();
		m_gameStack.top()->resetState(nullptr);
		m_gameStack.push(ss);
		m_gameStack.top()->Load();
		m_justSwaped = true;

	}
	if (m_gameStack.top()->getPushNPop() != nullptr)
	{
		State::pushNpop * pnp = m_gameStack.top()->getPushNPop();
		State * s = pnp->state;
		for (int i = 0; i < pnp->i; i++)
		{
			m_gameStack.top()->unLoad();
			delete m_gameStack.top();
			DX::g_deviceContext->ClearState();
			//std::this_thread::sleep_for(5s);
			m_gameStack.pop();
			m_gameStack.top()->pushNewState(nullptr);
		}
		m_gameStack.push(pnp->state);
		m_gameStack.top()->Load();
		delete pnp;
		m_justSwaped = true;

	}
	else if (m_gameStack.top()->getKillState())
	{
		m_gameStack.top()->unLoad();
		delete m_gameStack.top();
		DX::g_deviceContext->ClearState();
		//std::this_thread::sleep_for(5s);
		m_gameStack.pop();
		m_gameStack.top()->pushNewState(nullptr);
		m_gameStack.top()->Load();
		m_justSwaped = true;

	}

	if (m_gameStack.top()->getBackToMenu())
	{
		while (m_gameStack.size() > 1)
		{
			m_gameStack.top()->unLoad();
			delete m_gameStack.top();
			DX::g_deviceContext->ClearState();
			m_gameStack.pop();
			m_gameStack.top()->pushNewState(nullptr);
		}
		m_gameStack.top()->Load();
		m_justSwaped = true;

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
