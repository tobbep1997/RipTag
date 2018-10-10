#pragma once
#include "State.h"

class MainMenu : public State
{
private:


public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;
};