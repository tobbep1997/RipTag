#pragma once
#include "../Triggers/Triggerble.h"
class Door : public Triggerble
{
public:
	Door();
	~Door();

	// Inherited via Triggerble
	virtual void Triggerd(double deltaTime) override;
	virtual void unTriggerd(double deltaTime) override;
};

