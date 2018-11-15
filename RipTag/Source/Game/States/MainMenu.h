#pragma once
#include "State.h"
#include "../Loading Screen/LoadingScreen.h"
#include <filesystem>

class Quad;
class Circle;
class TextInput;
class MainMenu : public State
{
private:

	LoadingScreen m_loadingScreen;
	enum ButtonOrder
	{
		Play = 0,
		Lobby,
		Option,
		Quit
	};
	std::vector<Quad*> m_buttons;
	unsigned int m_currentButton;
	Quad * m_background = nullptr;
	FMOD::Channel * m_music;
	bool m_playstatePressed = false;

	Circle * c;
	long float cTimer;
public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;

	void StopMusic();

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
	void LoadAllGuiElements();
};