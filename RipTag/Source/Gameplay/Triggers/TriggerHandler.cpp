#include "TriggerHandler.h"



bool TriggerHandler::_triggerd(TriggerPairs * triggers)
{
	bool ret;
	if (triggers->seperate)
	{
		ret = false;
		for (int i = 0; i < triggers->triggers.size() && !ret; i++)
		{
			if (triggers->triggers[i]->Triggerd())
				ret = true;
		}
	}
	else
	{
		ret = true;
		for (int i = 0; i < triggers->triggers.size() && ret; i++)
			if (!triggers->triggers[i]->Triggerd())
				ret = false;
	}	
	return ret;
}

void TriggerHandler::_setTrigger(TriggerPairs * triggers, const bool & trigger, double deltaTime)
{
	for (int i = 0; i < triggers->triggerble.size(); i++)	
		if (trigger)
			triggers->triggerble[i]->Triggerd(deltaTime);
		else
			triggers->triggerble[i]->unTriggerd(deltaTime);
}

TriggerHandler::TriggerHandler()
{
}


TriggerHandler::~TriggerHandler()
{
	
	for (unsigned int i = 0; i < m_triggers.size(); i++)
	{
		delete m_triggers[i];
	}
	m_triggers.clear();
}

void TriggerHandler::Update(double deltaTime)
{	
	for (unsigned int i = 0;
		i < m_triggers.size();
		i++)
	{
		_setTrigger(m_triggers[i], _triggerd(m_triggers[i]), deltaTime);
	}
}

void TriggerHandler::AddPair(std::vector<Trigger*> & triggers, std::vector<Triggerble*> & triggerable, bool seperate)
{
	TriggerPairs * triggerPair = new TriggerPairs();
	triggerPair->triggers = triggers;
	triggerPair->triggerble = triggerable;
	triggerPair->seperate = seperate;
	m_triggers.push_back(triggerPair);
}
