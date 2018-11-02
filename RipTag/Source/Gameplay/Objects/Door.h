#pragma once
#include "../Triggers/Triggerble.h"
#include "../../Game/Actors/BaseActor.h"
class Door : public Triggerble, public BaseActor
{
private:
	DirectX::XMFLOAT4A pos1;
	DirectX::XMFLOAT4A pos2;
public:
	Door();
	~Door();

	// Inherited via Triggerble
	virtual void Triggerd(double deltaTime) override;
	virtual void unTriggerd(double deltaTime) override;

	void setPos(DirectX::XMFLOAT4A trigg, DirectX::XMFLOAT4A unTrigg);
};

