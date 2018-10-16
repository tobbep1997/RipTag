#pragma once
#include "../BaseActor.h"

class Teleport : public BaseActor
{
private:
	float m_charge = 0.0f;
	float m_travelSpeed = 10.0f;
	bool m_charging = false;
	bool m_gotActiveSphere = false;
public:
	Teleport();
	~Teleport();

	void chargeSphere();
	void ThrowSphere(DirectX::XMFLOAT4A startPos, DirectX::XMFLOAT4A Direction);
	DirectX::XMFLOAT4A TeleportToSphere();

	bool getActiveSphere() const;
	bool getCharging() const;
	void setCharging(bool charging);
};