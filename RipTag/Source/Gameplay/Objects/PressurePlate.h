#pragma once
#include "../Triggers/Trigger.h"
#include "../../Game/Actors/BaseActor.h"
class PressurePlate : public Trigger, public BaseActor
{
private:
	DirectX::XMFLOAT4A pos1;
	DirectX::XMFLOAT4A pos2;
public:
	PressurePlate();
	~PressurePlate();
		
	void Init();
	void BeginPlay() override;
	void Update(double deltaTime) override;

	void setPos(DirectX::XMFLOAT4A trigg, DirectX::XMFLOAT4A unTrigg);

};

