#pragma once
#include "Trigger.h"
#include "Triggerble.h"

#include <vector>

; class TriggerHandler
{
private:
	struct TriggerPairs
	{
		std::vector<Trigger*> triggers;
		std::vector<Triggerble*> triggerble;
	};
	std::vector<TriggerPairs *> m_triggers;

	bool _triggerd(TriggerPairs * triggers);
	void _setTrigger(TriggerPairs * triggers, const bool & trigger, double deltaTime);

public:
	TriggerHandler();
	~TriggerHandler();

	void Update(double deltaTime);

	void AddPair(std::vector<Trigger*> triggers, std::vector<Triggerble *> triggerable);
};

