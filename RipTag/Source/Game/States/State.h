#pragma once
#include "EngineSource/3D Engine/RenderingManager.h"

class State
{
public:
	struct pushNpop
	{
		pushNpop(int i, State * state)
		{
			this->i = i;
			this->state = state;
		}
		int i;
		State * state;
	};
private:
	bool m_killState;
	bool m_backToMenu;
	State * m_newState = nullptr;

	pushNpop * m_pnp;
protected:
	RenderingManager * p_renderingManager;
	const DirectX::XMFLOAT2 MIN_MAX_SLIDE = { 0.379f, 0.621f };

public:
	State(RenderingManager * rm = nullptr);
	virtual ~State();

	virtual void Load() = 0;
	virtual void unLoad() = 0;

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

	void BackToMenu();
	bool getBackToMenu();

	void pushAndPop(int i, State * state);
	void resetPushNPop();
	pushNpop * getPushNPop();
};
