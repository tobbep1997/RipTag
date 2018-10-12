#include "Game.h"
#include "Source/3D Engine/Extern.h"
#include "../../../InputManager/XboxInput/GamePadHandler.h"
#include "Source/Helper/Timer.h"


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

	//Input handler
	{
		GamePadHandler::Instance();
		InputHandler::REGISTER_TO_LUA();
	}
	Timer::Instance();

	//Network Start
	{
		pNetworkInstance = Network::Multiplayer::GetInstance();
		pNetworkInstance->Init();
		Network::Multiplayer::REGISTER_TO_LUA();
		Network::Packets::REGISTER_TO_LUA();
	}

	m_gameStack.push(new PlayState(m_renderingManager));
}

bool Game::isRunning()
{
	return m_renderingManager->getWindow().isOpen();
}

void Game::PollEvents()
{
	m_renderingManager->Update();

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
	pNetworkInstance->Update();
	
}

void Game::Draw()
{
	m_gameStack.top()->Draw();
}

void Game::ImGuiFrameStart()
{
	m_renderingManager->ImGuiStartFrame();
}

void Game::PushStateLUA(State * ptr)
{
	MainMenu * menuPtr = 0;
	PlayState * playPtr = 0;

	if (ptr)
	{
		menuPtr = dynamic_cast<MainMenu*>(ptr);
		if (menuPtr)
		{
			this->m_gameStack.push(menuPtr);
			return;
		}
		playPtr = dynamic_cast<PlayState*>(ptr);
		if (playPtr)
		{
			this->m_gameStack.push(playPtr);
			return;
		}
		//pause menu and lobby menu to add
	}
}

void Game::PopStateLUA()
{
	this->m_gameStack.pop();
}

void Game::REGISTER_TO_LUA(Game & gameInstance)
{

}

void Game::_handleStateSwaps()
{
	if (m_gameStack.top()->getKillState())
	{
		delete m_gameStack.top();
		m_gameStack.pop();
	}
}

void Game::_restartGameIf()
{
	if (InputHandler::isKeyPressed('B'))
	{
		if (isPressed == false)
		{
			delete m_gameStack.top();
			m_gameStack.pop();

			//Manager::g_meshManager.UnloadStaticMesh("KOMBIN");
			//Manager::g_meshManager.UnloadStaticMesh("SPHERE");

			//Manager::g_textureManager.UnloadTexture("KOMBIN");
			//Manager::g_textureManager.UnloadTexture("SPHERE");



			m_gameStack.push(new PlayState(m_renderingManager));
			isPressed = true;
		}
	}
	else
	{
		isPressed = false;
	}
}
