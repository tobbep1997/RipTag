#pragma once
#include "OptionState.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

class PauseMenu 
{
public:
	PauseMenu(); 
	~PauseMenu(); 

	void Update(double deltaTime, Camera* camera); 

	void Draw();

	const bool& getExitPause() const; 
	const bool& getMainMenuPressed() const; 

	void Load(); 
	void unLoad(); 

	void ExitPause() { m_exitPause = true; }

private:
	enum ButtonOrderMain
	{
		ReturnButton = 0,
		Options,
		MainMenuButton
	};

	enum ButtonOrderOptions
	{
		SliderFovButton = 0,
		SliderSensXButton,
		SliderSensYButton,
		SliderMasterButton,
		SliderEffectsButton,
		SliderAmbientButton,
		SliderMusicButton,
		Back
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

	enum Direction
	{
		None = 0,
		Up = -1,
		Down = 1,
		Right = -2,
		Left = 2
	};

private:
	const DirectX::XMFLOAT2 MIN_MAX_SLIDE_GENERAL		= { 0.05f, 0.35f };
	const DirectX::XMFLOAT2 MIN_MAX_SLIDE_SOUND			= { 0.60f, 0.95f }; 
	const DirectX::XMINT2 MIN_MAX_FOV								= { 45,     135   };
	const DirectX::XMINT2 MIN_MAX_SENSITIVITY					= { 1,       11     };
	const DirectX::XMINT2 MIN_MAX_SOUND                         = { 0,      100    };

	float m_stickTimerFOV     = 0;
	float m_stickTimerX			 = 0;
	float m_stickTimerY			 = 0;

	int m_fov;
	int m_master; 
	int m_effects; 
	int m_ambient; 
	int m_music; 

	DirectX::XMINT2 m_sens;
	LastInputUsed m_liu  = Mouse;

	short m_currentButton	= 0;

	bool m_sliderPressed              = false;
	bool m_buttonPressed			= false;
	bool m_mainMenuPressed		= false; 
	bool m_mouseMoved             = false;
	bool m_exitPause                    = false; 
	bool m_inOptions					= false;

	std::vector<Quad* > m_buttonsMain;
	std::vector<Quad*> m_buttonsOptions;
	std::vector<Quad* > m_text;
	Quad * m_fullscreenOverlay = nullptr;

private:
	void _initButtons();

	void _slideSound();
	void _slideGeneral();

	bool _handleMouseInput();
	void _handleGamePadInput(double dt);
	void _handleKeyboardInput(double dt);
	
	void _handleMouseInOptions(DirectX::XMFLOAT2 & mousePos);
	void _handleMouseInMain(DirectX::XMFLOAT2 & mousePos);
	
	void _handleInputInOptions(int dir);
	void _inOptionsOnUp();
	void _inOptionsOnDown();
	void _inOptionsOnRight();
	void _inOptionsOnLeft();

	void _handleInputInMain(int dir);
	void _inMainOnUp();
	void _inMainOnDown();

	void _onButtonUsed();
	void _onButtonSwap(int newButton);

	void _WriteSettingsToFile();
	void _ReadSettingsFromFile();
	void _ParseFileInputInt(const std::string & str, int key);

	void _updateMain();
	void _updateOptions(float deltaTime, Camera * camera);

	void _updateOptionsText();
	void _updateFOV(float deltaTime, Camera * camera);
	void _updateSenseX(float deltaTime);
	void _updateSenseY(float deltaTime);
	void _updateMasterAudio();
	void _updateEffectAudio();
	void _updateAmbientAudio();
	void _updateMusicAudio();
	void _updateBackToMain();
};