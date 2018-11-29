#include "RipTagPCH.h"
#include "PauseMenu.h"

PauseMenu::PauseMenu()
{
}

PauseMenu::~PauseMenu()
{
}

void PauseMenu::Update(double deltaTime)
{
	//Specific update for PauseMenu 

	//Base Update (if needed). 
	OptionState::Update(deltaTime); 
}

void PauseMenu::Draw()
{
}

void PauseMenu::Load()
{
}

void PauseMenu::unLoad()
{
}
