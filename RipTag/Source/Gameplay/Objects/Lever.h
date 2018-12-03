#pragma once
#include "../Triggers/Trigger.h"
#include "../../Game/Actors/BaseActor.h"
#include "../../Physics/Wrapper/RayCastListener.h"

class Lever : public Trigger
{
public:
	Lever();
	Lever(int uniqueId, int linkedID, bool isTrigger);
	~Lever();

	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll);
	void Update(double deltaTime);
	bool isEqual(Lever* target);
	void BeginPlay() override;
	void handleContact(RayCastListener::RayContact* contact);
private:
	bool m_interacted = false;
	void _playSound(AudioEngine::SoundDesc * soundDesc) override;
};

