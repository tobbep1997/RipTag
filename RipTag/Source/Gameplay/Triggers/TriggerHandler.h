#pragma once
#include <vector>

class Trigger;
class Triggerable;

class TriggerHandler
{
private:
	
public:
	struct TriggerPair
	{
		std::vector<Trigger*>  triggers;
		std::vector<Triggerable*> triggerables;
		~TriggerPair()
		{
			triggers.clear();
			triggerables.clear();
		}
	};
	std::map<int, Trigger*> netWorkTriggers;
	std::map<int, TriggerPair> localTriggerPairs;
	TriggerHandler();
	~TriggerHandler();

	void Update(double deltaTime);
	void Draw();
	std::vector<Trigger*> Triggers;
	std::vector<Triggerable*> Triggerables;


	void Release();
	void LoadTriggerPairMap();

};

