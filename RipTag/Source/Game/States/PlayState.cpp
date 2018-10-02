#include "PlayState.h"
#include "../../../../InputManager/XboxInput/GamePadHandler.h"
#include "../../Input/Input.h"

PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	CameraHandler::Instance();
	
	player = new Player();
	enemy = new Enemy();
	CameraHandler::setActiveCamera(player->getCamera());

	m_world = new b3World();
}

PlayState::~PlayState()
{
	delete enemy;
	delete player;
	delete m_world;
}

void PlayState::Update(double deltaTime)
{
	if (GamePadHandler::IsAPressed())
	{
		Input::ForceDeactivateGamepad();
	}

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
