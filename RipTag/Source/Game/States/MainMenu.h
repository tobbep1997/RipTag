#pragma once
#include "State.h"
#include "2D Engine/Quad/Quad.h"


class MainMenu : public State
{
private:
	Quad * playButton;
	Quad * quitButton;


public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;
};