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

bool State::getKillState()
{
	return m_killState;
}

void State::setKillState(const bool killState)
{
	m_killState = killState;
}
