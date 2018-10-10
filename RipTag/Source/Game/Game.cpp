#include "Game.h"
#include "Source/3D Engine/Extern.h"
#include "../../../InputManager/XboxInput/GamePadHandler.h"
#include "Source/Helper/Timer.h"


Game::Game()
{
	
}

Game::~Game()
{
	m_renderingManager.Release();
	while(!m_gameStack.empty())
	{
		delete m_gameStack.top();
		m_gameStack.pop();
	}
}

void Game::Init(_In_ HINSTANCE hInstance)
{
	m_renderingManager.Init(hInstance);
	m_gameStack.push(new PlayState(&m_renderingManager));

	GamePadHandler::Instance();
	Timer::Instance();
}

bool Game::isRunning()
{
	return m_renderingManager.getWindow().isOpen();
}

void Game::PollEvents()
{
	m_renderingManager.Update();

}

void Game::Clear()
{
	//TODO Fix clear
	m_renderingManager.Clear();
}

void Game::Update(double deltaTime)
{
#if _DEBUG
	_restartGameIf();
#endif
	_handleStateSwaps();
	GamePadHandler::UpdateState();
	m_gameStack.top()->Update(deltaTime);
	
}

void Game::Draw()
{
	m_gameStack.top()->Draw();
}

void Game::ImGuiFrameStart()
{
	m_renderingManager.ImGuiStartFrame();
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

			Manager::g_meshManager.UnloadStaticMesh("KOMBIN");
			Manager::g_meshManager.UnloadStaticMesh("SPHERE");

			Manager::g_textureManager.UnloadTexture("KOMBIN");
			Manager::g_textureManager.UnloadTexture("SPHERE");



			m_gameStack.push(new PlayState(&m_renderingManager));
			isPressed = true;
		}
	}
	else
	{
		isPressed = false;
	}
}
