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

	float m_stickTimer = 0; 

	Quad * m_FmodLogo = nullptr;
	Quad * m_RakNetLogo = nullptr;

	bool m_gamePadInUse = false;
public:
	MainMenu(RenderingManager * rm);
	~MainMenu();

	void Update(double deltaTime) override;

	void Draw() override;

	void StopMusic();

private:
	void _initButtons();
	bool _handleGamePadInput(float deltaTime);
	void _handleKeyboardInput();
	bool _handleMouseInput();
	void _updateSelectionStates();
	void _resetButtons();
	void _handleMousePP();

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;
	void LoadAllGuiElements();
};