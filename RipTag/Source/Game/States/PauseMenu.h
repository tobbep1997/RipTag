#pragma once
#include "OptionState.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

class PauseMenu 
{
private:
	
	enum ButtonOrder
	{
		SliderFovButton = 0,
		SliderSensXButton,
		SliderSensYButton,
		SliderMasterButton,
		SliderEffectsButton,
		SliderAmbientButton,
		SliderMusicButton,
		ReturnButton,
		MainMenuButton
	};

	enum TextOrder
	{
		SlideBarFov = 0,
		SliderBarSensX, 
		SliderBarSensY,
		SliderBarMaster,
		SliderBarEffects,
		SliderBarAmbient,
		SliderBarMusic,
		SliderFov,
		SliderSensitivityX,
		SliderSensitivityY,
		SliderMaster,
		SliderEffects, 
		SliderAmbient,
		SliderMusic
	};

	enum LastInputUsed
	{
		Mouse,
		Gamepad,
		Keyboard
	};

	float m_stickTimerFOV = 0;
	float m_stickTimerX = 0;
	float m_stickTimerY = 0;

	std::vector<Quad* > m_buttons;
	std::vector<Quad* > m_text;
	int m_fov;
	int m_master; 
	int m_effects; 
	int m_ambient; 
	int m_music; 
	DirectX::XMINT2 m_sens;

	bool m_sliderPressed;
	short m_currentButton;
	bool m_buttonPressed;

	bool m_mainManuPressed = false; 

	bool m_mouseMoved;
	LastInputUsed m_liu;

	bool m_exitPause = false; 

	const DirectX::XMFLOAT2 MIN_MAX_SLIDE_GENERAL = { 0.05f, 0.35f };
	const DirectX::XMFLOAT2 MIN_MAX_SLIDE_SOUND = { 0.60f, 0.95f}; 
	const DirectX::XMINT2 MIN_MAX_FOV = { 45, 135 };
	const DirectX::XMINT2 MIN_MAX_SENSITIVITY = { 1, 11 };
	const DirectX::XMINT2 MIN_MAX_SOUND = { 0, 100 };


	void _slideSound();
	void _slideGeneral();
	void _initButtons();
	void _handleGamePadInput(double dt);
	void _handleKeyboardInput(double dt);
	bool _handleMouseInput();
	void _updateSelectionStates();
	void _WriteSettingsToFile();
	void _ReadSettingsFromFile();
	void _ParseFileInputInt(const std::string & str, int key);

public:
	PauseMenu(); 
	~PauseMenu(); 

	void Update(double deltaTime, Camera* camera); 

	void Draw();

	const bool& getExitPause() const; 
	const bool& getMainMenuPressed() const; 

	void Load(); 
	void unLoad(); 
};