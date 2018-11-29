#include "RipTagPCH.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu()
{

}

PauseMenu::~PauseMenu()
{
	//Yeah
}

void PauseMenu::Update(double deltaTime)
{
	if (!InputHandler::getShowCursor())
		InputHandler::setShowCursor(TRUE);

}

void PauseMenu::Draw()
{
	m_HUDcomp.HUDDraw(); 
}

void PauseMenu::_slide()
{
}

void PauseMenu::_initButtons()
{

}

void PauseMenu::_handleGamePadInput(double dt)
{
}

void PauseMenu::_handleKeyboardInput(double dt)
{
}

bool PauseMenu::_handleMouseInput()
{
	return false;
}

void PauseMenu::_updateSelectionStates()
{
}

void PauseMenu::_WriteSettingsToFile()
{
}

void PauseMenu::_ReadSettingsFromFile()
{
}

void PauseMenu::_ParseFileInputInt(const std::string & str, int key)
{
}

void PauseMenu::Load()
{
	//TODO
	//Load all the HUD for the Pause menu.
	m_HUDcomp.InitHUDFromFile("../PauseMenu.txt");
}

void PauseMenu::unLoad()
{
	//TODO
	//Push all the HUD elements to the quad queue, make sure they are drawn over the crosshair and ecventual other HUD.  
}
