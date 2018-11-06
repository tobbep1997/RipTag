#pragma once
#include "../Triggers/Trigger.h"
#include "../../Game/Actors/BaseActor.h"

class Lever : public Trigger, public BaseActor
{
public:
	Lever();
	~Lever();

	std::string unlock, lock;

	void Init();
	void BeginPlay() override;
	void Update(double deltaTime) override;
};

