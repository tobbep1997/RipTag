#pragma once
#include "State.h"
#include "2D Engine/Quad/Quad.h"


class MainMenu : public State
{
private:
	bool m_playPrev;
	bool m_playCurrent;
private:
	Quad * playButton;

public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;
};