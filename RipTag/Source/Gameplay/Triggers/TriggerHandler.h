#pragma once
#include <vector>

class Trigger;
class Triggerble;

class TriggerHandler
{
private:
	struct TriggerPairs
	{
		bool seperate;
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

	void AddPair(std::vector<Trigger*> & triggers, std::vector<Triggerble *> & triggerable, bool seperate = false);
};

