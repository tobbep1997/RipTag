#pragma once
#include "../Triggers/Trigger.h"
#include "../../Game/Actors/BaseActor.h"

class Lever : public Trigger
{
public:
	Lever();
	Lever(int uniqueId, int linkedID, bool isTrigger);
	~Lever();

	std::string unlock, lock;

	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll);
	void Update(double deltaTime);
	bool isEqual(Lever* target);
	void BeginPlay() override;
};

