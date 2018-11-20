#pragma once
#pragma once
#include "State.h"
#include <vector>
class Quad;
class OptionSound : public State
{
private:
	enum ButtonOrder
	{
		SliderMaster = 0,
		SliderEffects,
		SliderAmbient,
		SliderMusic,
		Return
	};
	enum LastInputUsed
	{
		Mouse,
		Gamepad,
		Keyboard
	};

	const DirectX::XMINT2 MIN_MAX_SOUND = { 0, 100 };

	std::vector<Quad* > m_buttons;
	std::vector<Quad* > m_text;
	Quad* m_background = nullptr;
	Quad * m_restart;
	
	int m_master;
	int m_effects;
	int m_ambient;
	int m_music;

	bool m_sliderPressed;
	short m_currentButton;
	bool m_buttonPressed;

	bool m_mouseMoved;
	LastInputUsed m_liu;
public:
	OptionSound(RenderingManager * rm = nullptr);
	~OptionSound();

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