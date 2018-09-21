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

void Game::Init(_In_ HINSTANCE hInstance)
{
	m_renderingManager.Init(hInstance);
	m_gameStack.push(new PlayState(&m_renderingManager));
	modelManager.addStaticMesh("../Assets/KUB.bin");
	//modelManager.staticMesh[0]->setScale(10, 1, 10);
	modelManager.m_staticMesh[0]->setScale(10, 1, 10);
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

void Game::Update()
{
	m_gameStack.top()->Update();
	
}

void Game::Draw()
{
	modelManager.DrawMeshes();
	m_gameStack.top()->Draw();
}

void Game::ImGuiFrameStart()
{
	m_renderingManager.ImGuiStartFrame();
}
