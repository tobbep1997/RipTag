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
	//TODO::FiX

	//m_dynamicObjectContainer.Draw();
	//m_level.Draw();
	// Cam camera = getCam();
	//p_renderingManager->Flush(camera);
}
