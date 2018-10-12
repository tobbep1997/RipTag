#include "Teleport.h"

Teleport::Teleport() : BaseActor()
{
}

Teleport::~Teleport()
{
}

void Teleport::chargeSphere()
{
	m_charging = true;
	if (m_charge < 2.0f)
		m_charge += 0.05;
}

void Teleport::ThrowSphere(DirectX::XMFLOAT4A StartPos, DirectX::XMFLOAT4A Direction)
{
	DirectX::XMFLOAT4A forward = Direction;
	DirectX::XMFLOAT4A startPos(
		StartPos.x + forward.x,
		StartPos.y + forward.y,
		StartPos.z + forward.z,
		1.0f
	);
	
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat4A(&forward);
	dir = DirectX::XMVectorScale(dir, (m_travelSpeed * m_charge));
	DirectX::XMStoreFloat4A(&forward, dir);

	// Set the teleporter origin to the player
	setPosition(startPos.x, startPos.y, startPos.z);
	setLiniearVelocity(forward.x, forward.y, forward.z);

	m_gotActiveSphere = true;
	// Reset the charge
	m_charge = 0.0f;
}

DirectX::XMFLOAT4A Teleport::TeleportToSphere()
{
	DirectX::XMFLOAT4A teleportPos = getPosition();
	setPosition(-100.0f, -100.0f, -100.0f);
	m_gotActiveSphere = false;
	return teleportPos;
}

bool Teleport::getActiveSphere() const
{
	return m_gotActiveSphere;
}

bool Teleport::getCharging() const
{
	return m_charging;
}

void Teleport::setCharging(bool charging)
{
	m_charging = charging;
}
