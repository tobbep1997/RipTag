#include "State.h"

State::State(RenderingManager * rm)
{
	p_renderingManager = rm;
}

State::~State()
{
	p_renderingManager = nullptr;
}
