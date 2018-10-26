#pragma once
#include "EngineSource/3D Engine/RenderingManager.h"

#define LUA_STATE_METATABLE "STATE"
class State
{
private:
	bool m_killState;
	State * m_newState = nullptr;
protected:
	RenderingManager * p_renderingManager;
	

public:
	State(RenderingManager * rm = nullptr);
	virtual ~State();

	virtual void Update(double deltaTime) = 0;

	virtual void Draw() = 0;

	//----------------
	//This is used to push a state from a state
	virtual State * getNewState();
	void pushNewState(State * state);
	//----------------
	//This is the functions used to kill a state
	bool getKillState();	//Gets the curreent killState
	void setKillState(const bool killState = true);
};
