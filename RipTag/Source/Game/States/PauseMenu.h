#pragma once
#include "OptionState.h"
#include "2D Engine/Quad/Components/HUDComponent.h"
class PauseMenu
{
private:
	
	enum ButtonOrder
	{
		SliderFov = 0,
		SliderSensitivityX,
		SliderSensitivityY,
		SoundSettings,
		Return
	};

	enum LastInputUsed
	{
		Mouse,
		Gamepad,
		Keyboard
	};


	HUDComponent m_HUDcomp; 

	float m_stickTimerFOV = 0;
	float m_stickTimerX = 0;
	float m_stickTimerY = 0;

	std::vector<Quad* > m_buttons;
	std::vector<Quad* > m_text;
	Quad* m_background = nullptr;
	Quad * m_restart;
	int m_fov;
	DirectX::XMINT2 m_sens;

	bool m_sliderPressed;
	short m_currentButton;
	bool m_buttonPressed;

	bool m_mouseMoved;
	LastInputUsed m_liu;
	
public:
	PauseMenu(); 
	~PauseMenu(); 

	void Update(double deltaTime); 

	void Draw();

	void _slide();
	void _initButtons();
	void _handleGamePadInput(double dt);
	void _handleKeyboardInput(double dt);
	bool _handleMouseInput();
	void _updateSelectionStates();
	void _WriteSettingsToFile();
	void _ReadSettingsFromFile();
	void _ParseFileInputInt(const std::string & str, int key);
	
	void Load(); 
	void unLoad(); 
};