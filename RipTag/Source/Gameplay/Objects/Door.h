#pragma once
#include "../Triggers/Triggerble.h"
#include "EngineSource/3D Engine/Components/Base/Drawable.h"
class Door : public Triggerble, public Drawable
{
public:
	Door();
	~Door();

	// Inherited via Triggerble
	virtual void Triggerd(double deltaTime) override;
	virtual void unTriggerd(double deltaTime) override;
};

