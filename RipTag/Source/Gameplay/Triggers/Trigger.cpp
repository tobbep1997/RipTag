#include "RipTagPCH.h"
#include "Trigger.h"



void Trigger::p_trigger(const bool & trigger)
{
	this->m_triggerd = trigger;
}

Trigger::Trigger()
{
}

Trigger::~Trigger()
{
}

const bool & Trigger::Triggerd() const
{
	return m_triggerd;
}
