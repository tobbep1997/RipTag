#pragma once
#include "State.h"

class Quad;

class MainMenu : public State
{
private:
	enum ButtonOrder
	{
		Play = 0,
		Lobby = 1,
		Quit = 2
	};
	std::vector<Quad*> m_buttons;
	unsigned int m_currentButton;

public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;

private:
	void _initButtons();
	void _handleGamePadInput();
	void _handleKeyboardInput();
	void _handleMouseInput();
	void _updateSelectionStates();
	void _resetButtons();
};