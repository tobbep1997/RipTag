#include "Game.h"

Game::Game()
{
}

Game::~Game()
{
	m_renderingManager.Release();
}

void Game::Init(HINSTANCE hInstance)
{
	m_renderingManager.Init(hInstance);
}

bool Game::isRunning()
{
}

void Game::PollEvents()
{
	m_renderingManager.Update();
}

void Game::Clear()
{
	//TODO Fix clear
}

void Game::Update()
{
	
}

void Game::Draw()
{

}
