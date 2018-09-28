#include "PlayState.h"

PlayState::PlayState(RenderingManager * rm) : State(rm)
{	

	CameraHandler::Instance();
	player = new Player();
	CameraHandler::setActiveCamera(player->getCamera());
}

PlayState::~PlayState()
{
	delete player;
}

void PlayState::Update(double deltaTime)
{
	player->Update(deltaTime);
	//_tempCameraControls();
	

	m_objectHandler.Update();
	m_levelHandler.Update();
}

void PlayState::Draw()
{
	/*Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -6);*/
	//TODO::FiX

	//m_dynamicObjectContainer.Draw();
	//m_level.Draw();
	// Cam camera = getCam();
	//p_renderingManager->Flush(camera);
	m_objectHandler.Draw();
	m_levelHandler.Draw();

	p_renderingManager->Flush(*CameraHandler::getActiveCamera());
}
