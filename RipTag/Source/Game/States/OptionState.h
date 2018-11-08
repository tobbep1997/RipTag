#pragma once
#include "State.h"
#include <vector>
class Quad;

class OptionState : public State
{
private:
	std::vector<Quad* > m_buttons;

	
public:
	OptionState(RenderingManager * rm = nullptr);
	~OptionState();

	void Update(double deltaTime) override;
	void Draw() override;
private:
	void _initButtons();
	void _handleGamepadInput();
	void _handleKeyboardInput();
	void _handleMouseInput();
	void _updateSelectionStates();
};