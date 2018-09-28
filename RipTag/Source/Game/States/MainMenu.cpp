#include "MainMenu.h"

MainMenu::MainMenu(RenderingManager * rm) : State(rm)
{
}

MainMenu::~MainMenu()
{
}

void MainMenu::Update(double deltaTime)
{
}

void MainMenu::Draw()
{
	Camera camera = Camera(DirectX::XM_PI * 0.5f, 16.0f / 9.0f);
	camera.setPosition(0, 0, -10);
	//TODO::FiX
	p_renderingManager->Flush(camera);
}
