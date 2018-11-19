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
	std::string activatedAnimation;
	std::string deactivatedAnimation;

protected:
	void p_trigger(const bool & trigger);
public:
	Trigger();
	Trigger(int uniqueId, int linkedID, bool isTrigger, std::string activeAnim, std::string deactiveAnim);
	virtual ~Trigger();
	void setUniqueId(int id) { m_uniqueID = id; }
	int getLinkId() { return this->m_linkedID; };
	int getUniqueID() { return this->m_uniqueID; };
	bool getIsTriggerable() { return this->m_isTrigger; };
	const bool & Triggered() const;
	void setTriggerState(bool state, bool isLocal = true);
	bool getTriggerState() { return m_triggerState; }

	void BeginPlay() override;
	void Update(double deltaTime) override;

	void Release();

	void SendOverNetwork();
private:
	virtual void _playSound(AudioEngine::SoundType st = AudioEngine::Player) = 0;
};

