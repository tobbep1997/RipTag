#pragma once
#include "../Triggers/Trigger.h"
#include "../../Physics/Wrapper/PhysicsComponent.h"
#include "../../Game/Actors/BaseActor.h"
class PressurePlate : public Trigger, public BaseActor
{
private:
	
public:
	PressurePlate();
	~PressurePlate();
		
	void Init();
	void BeginPlay() override;
	void Update(double deltaTime) override;
};

