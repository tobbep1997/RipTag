#include "PlayState.h"

PlayState::PlayState(RenderingManager * rm) : State(rm)
{

}

PlayState::~PlayState()
{
}

void PlayState::Update()
{
	
}

void PlayState::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -6);
	//TODO::FiX
	p_renderingManager->Flush(camera);
	//m_dynamicObjectContainer.Draw();
	//m_level.Draw();
	// Cam camera = getCam();
	//p_renderingManager->Flush(camera);
}
