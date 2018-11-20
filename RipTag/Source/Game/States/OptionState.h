#pragma once
#include "State.h"
#include <vector>
class Quad;
class OptionState : public State
{
private:
	enum ButtonOrder
	{
		SliderFov = 0,
		SliderSensitivityX,
		SliderSensitivityY,
		ToggleResolution,
		ToggleGraphics,
		ToggleFullscreen,
		SoundSettings,
		Return
	};
	enum LastInputUsed
	{
		Mouse,
		Gamepad,
		Keyboard
	};

	const DirectX::XMINT2 MIN_MAX_FOV = {45, 135};
	const DirectX::XMINT2 MIN_MAX_SENSITIVITY = {1, 11};
	const std::string SWAP_RESOLUTION[3] = {
		"Res: 1280 x 720",
		"Res: 1920 x 1080",
		"Res: 3840 x 2160"
	};
	const std::string SWAP_GRAPHICS[3] = {
		"Graphics: LOW",
		"Graphics: MEDIUM",
		"Graphics: HIGH"
	};
	const DirectX::XMINT2 RES[3] = {
		{1280, 720},
		{1920, 1080},
		{3840, 2160}
	};

	std::vector<Quad* > m_buttons;
	std::vector<Quad* > m_text;
	Quad* m_background = nullptr;
	Quad * m_restart;
	int m_fov;
	DirectX::XMINT2 m_sens;
	unsigned short m_resSelection;
	unsigned short m_graphicsSelection;
	bool m_sliderPressed;
	bool m_fullscreen;
	short m_currentButton;
	bool m_buttonPressed;
	bool m_drawMustRestart;
	bool m_mouseMoved;
	LastInputUsed m_liu;
public:
	OptionState(RenderingManager * rm = nullptr);
	~OptionState();

	void Update(double deltaTime) override;
	void Draw() override;
private:
	void _slide();
	void _initButtons();
	void _handleGamePadInput(double dt);
	void _handleKeyboardInput(double dt);
	bool _handleMouseInput();
	void _updateSelectionStates();
	void _WriteSettingsToFile();
	void _ReadSettingsFromFile();
	void _ParseFileInputInt(const std::string & str, int key);

	// Inherited via State
	virtual void Load() override;
	virtual void unLoad() override;
};