#pragma once
#include "State.h"

class MainMenu : public State
{
private:


public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update() override;

	void Draw() override;
};