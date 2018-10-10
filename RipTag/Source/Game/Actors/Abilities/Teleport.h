#pragma once
#include "../../../Physics/Wrapper/PhysicsComponent.h"
#include "../BaseActor.h"

class Teleport : public BaseActor
{
private:
	float m_travelSpeed = 10.0f;
	bool m_gotActiveSphere = false;
public:
	Teleport();
	~Teleport();

	void ThrowSphere(DirectX::XMFLOAT4A startPos, DirectX::XMFLOAT4A Direction);
	DirectX::XMFLOAT4A TeleportToSphere();

	bool getActiveSphere() const;
	void setActiveSphere(bool active);


};