#pragma once
#include "../Triggers/Triggerble.h"
#include "../../Game/Actors/BaseActor.h"
class Door : public Triggerble, public BaseActor
{
public:
	Door();
	~Door();

	// Inherited via Triggerble
	virtual void Triggerd(double deltaTime) override;
	virtual void unTriggerd(double deltaTime) override;
};

