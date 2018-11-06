#pragma once
#include "../Triggers/Trigger.h"

class PressurePlate : public Trigger
{
private:
	DirectX::XMFLOAT4A pos1;
	DirectX::XMFLOAT4A pos2;
public:
	PressurePlate();
	PressurePlate(int uniqueId, int linkedID, bool isTrigger);
	~PressurePlate();
		
	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll);
	void Update(double dt);

	void setPos(DirectX::XMFLOAT4A trigg, DirectX::XMFLOAT4A unTrigg);

};

