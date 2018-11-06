#pragma once
#include "../../Game/Actors/BaseActor.h"
#include <Multiplayer.h>

class Trigger : public BaseActor
{
private:
	bool m_triggerState = false;
	bool m_isTrigger = false;
	int m_linkedID;
	int m_uniqueID;
protected:
	void p_trigger(const bool & trigger);
public:
	Trigger();
	Trigger(int uniqueId, int linkedID, bool isTrigger);
	virtual ~Trigger();
	int getLinkId() { return this->m_linkedID; };
	int getUniqueID() { return this->m_uniqueID; };
	bool getIsTriggerable() { return this->m_isTrigger; };
	const bool & Triggered() const;
	void setTriggerState(bool state) { m_triggerState = state; }
	bool getTriggerState() { return m_triggerState; }

	void BeginPlay() override;
	void Update(double deltaTime) override;

	void Release();

	void SendOverNetwork();
};

