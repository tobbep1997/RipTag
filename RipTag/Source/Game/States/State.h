#pragma once
//#include "Source/3D Engine/RenderingManager.h"

//#include "../../../Engine/Source/3D Engine/RenderingManager.h"
//#include "Source/3D Engine/RenderingManager.h"
#include "../../../../Engine/Source/3D Engine/RenderingManager.h"
class State
{
private:
	bool m_killState;

protected:
	RenderingManager * p_renderingManager;
	
public:
	State(RenderingManager * rm = nullptr);
	virtual ~State();

	virtual void Update(double deltaTime) = 0;

	virtual void Draw() = 0;

	//----------------
	//This is the functions used to kill a state
	bool getKillState();	//Gets the curreent killState
	void setKillState(const bool killState = true);
};
