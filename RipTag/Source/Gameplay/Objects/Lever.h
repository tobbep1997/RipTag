#pragma once
#include "../Triggers/Trigger.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "../../Game/Actors/BaseActor.h"

class Lever : public Trigger, public BaseActor
{
public:
	Lever();
	~Lever();

	void Init();
	void BeginPlay() override;
	void Update(double deltaTime) override;
	bool isEqual(Lever* target);
};

