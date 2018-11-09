#include "RipTagPCH.h"
#include "LoseState.h"


LoseState::LoseState(RenderingManager * rm) : State(rm)
{
}


LoseState::~LoseState()
{
}

void LoseState::Update(double deltaTime)
{
	std::cout << "you lost" << std::endl;
	if (InputHandler::isKeyPressed(InputHandler::Spacebar))
	{
		BackToMenu();
	}
}

void LoseState::Draw()
{

	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);
	p_renderingManager->Flush(camera);
}
