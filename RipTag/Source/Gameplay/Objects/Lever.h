#pragma once
#include "../Triggers/Trigger.h"
#include "../../Game/Actors/BaseActor.h"

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
private:
	void _playSound(AudioEngine::SoundType st = AudioEngine::Player) override;
};

