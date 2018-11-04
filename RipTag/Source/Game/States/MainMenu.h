#pragma once
#include "State.h"

class Quad;

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