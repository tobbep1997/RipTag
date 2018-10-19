#include "State.h"



State::State(RenderingManager * rm)
{
	p_renderingManager = rm;
	m_killState = false;
}

State::~State()
{
	p_renderingManager = nullptr;
}

State * State::getNewState()
{
	State * newState = m_newState;
	return newState;
}

void State::pushNewState(State * state)
{
	m_newState = state;
}

bool State::getKillState()
{
	return m_killState;
}

void State::setKillState(const bool killState)
{
	m_killState = killState;
}
