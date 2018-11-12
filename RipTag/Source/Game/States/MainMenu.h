#pragma once
#include "State.h"

class Quad;
class Circle;
class TextInput;
class MainMenu : public State
{
private:
	enum ButtonOrder
	{
		Play = 0,
		Lobby,
		Option,
		Quit
	};
	std::vector<Quad*> m_buttons;
	unsigned int m_currentButton;


	Circle * c;
	long float cTimer;
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

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;
};