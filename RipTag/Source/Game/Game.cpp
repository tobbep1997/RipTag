#include "Game.h"
#include "Source/3D Engine/Extern.h"


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


	Manager::g_meshManager.loadStaticMesh("SCENE");
	Manager::g_textureManager.loadTextures("SPHERE");
	modelManager.addNewModel(Manager::g_meshManager.getStaticMesh("SCENE"), Manager::g_textureManager.getTexture("SPHERE"));
	
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
	_handleStateSwaps();

	m_gameStack.top()->Update(deltaTime);
	
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

void Game::_handleStateSwaps()
{
	if (m_gameStack.top()->getKillState())
	{
		delete m_gameStack.top();
		m_gameStack.pop();
	}
}
