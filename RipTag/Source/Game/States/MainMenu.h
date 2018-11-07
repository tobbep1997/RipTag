#pragma once
#include "State.h"

class Quad;
class Circle;

class MainMenu : public State
{
private:
	Quad * playButton;
	Quad * quitButton;

	Circle * c;
	long float cTimer;
public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;
};