#include "PlayState.h"

PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	CameraHandler::Instance();
	player = new Player();
	enemy = new Enemy();
	CameraHandler::setActiveCamera(player->getCamera());
}

PlayState::~PlayState()
{
	delete enemy;
	delete player;
}

void PlayState::Update(double deltaTime)
{
	player->Update(deltaTime);
	enemy->Update(deltaTime);
	
	m_objectHandler.Update();
	m_levelHandler.Update();
}

void PlayState::Draw()
{
	m_objectHandler.Draw();
	m_levelHandler.Draw();

	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
}
