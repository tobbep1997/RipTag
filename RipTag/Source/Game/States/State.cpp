#include "RipTagPCH.h"
#include "State.h"



State::State(RenderingManager * rm)
{
	p_renderingManager = rm;
	m_killState = false;
	m_backToMenu = false;
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

void State::BackToMenu()
{
	m_backToMenu = true;
}

bool State::getBackToMenu()
{
	return m_backToMenu;
}

void State::pushAndPop(int i, State* state)
{
	this->m_pnp = new pushNpop(i, state);
}

void State::resetState(State* state)
{
	this->m_resetState = state;
}

void State::resetPushNPop()
{
	delete this->m_pnp;
	this->m_pnp = nullptr;
}

State::pushNpop * State::getPushNPop()
{
	return this->m_pnp;
}

State* State::GetReset()
{
	return this->m_resetState;
}
