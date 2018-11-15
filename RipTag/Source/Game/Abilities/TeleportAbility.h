#pragma once

#include "AbilityComponent.h"
#include "../Actors/BaseActor.h"
#include "2D Engine/Quad/Components/HUDComponent.h"
class PointLight;

class TeleportAbility : public AbilityComponent, public BaseActor , public HUDComponent
{
private: // CONST VARS
	const int START_MANA_COST = 10;
	const float TRAVEL_SPEED = 20.0f;
	const float MAX_CHARGE = 2.0f;
	const bool USE_SHADOWS = true;
	const DirectX::XMFLOAT4A COLOUR{
		0.3f,
		1.0f,
		1.0f,
		1.0f
	};
private:
	// ENUM
	enum TeleportState
	{
		Throwable,		// Ready to charge
		Charging,		// Charging
		Teleportable,	// Ready to tp
		Cooldown,		// Just teleported and can not throw
		RemoteActive    //this is unique for network and the remote state machine
	};
private:
	TeleportState	m_tpState;
	float			m_charge;
	float			m_travelSpeed;
	PointLight*		m_light;

	Quad * m_bar;

	bool m_canceled = false;
	//for network
	DirectX::XMFLOAT4A m_lastStart;
	DirectX::XMFLOAT4A m_lastVelocity;

	DirectX::BoundingSphere * m_boundingSphere;

public:
	TeleportAbility(void * owner = nullptr);
	~TeleportAbility();

	void Init() override;
	void Use() override { return; }
	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;
	void UpdateFromNetwork(Network::ENTITYABILITYPACKET * data) override;

	void Draw() override;

	unsigned int getState();
	DirectX::XMFLOAT4A getVelocity();
	DirectX::XMFLOAT4A getStart();

	DirectX::BoundingSphere * GetBoundingSphere() const;

private:
	// Private functions
	void _logicLocal(double deltaTime);

	void _inStateThrowable();
	void _inStateCharging(double dt);
	void _inStateTeleportable();
	void _inStateCooldown(double dt);

	void _updateLight();
};