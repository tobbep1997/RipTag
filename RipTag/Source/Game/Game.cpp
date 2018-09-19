#include "Game.h"

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

void Game::Init(HINSTANCE hInstance)
{
	m_renderingManager.Init(hInstance);
}

bool Game::isRunning()
{
	m_renderingManager.getWindow().isOpen();
}

void Game::PollEvents()
{
	m_renderingManager.Update();
}

void Game::Clear()
{
	//TODO Fix clear
	//m_renderingManager.clear(); typ isch
}

void Game::Update()
{
	m_gameStack.top()->Update();
}

void Game::Draw()
{
	m_gameStack.top()->Draw();
}
