#pragma once
#include "AbilityComponent.h"
#include "../Actors/BaseActor.h"
#include "2D Engine/Quad/Components/HUDComponent.h"


class TeleportAbility : public AbilityComponent, public BaseActor , public HUDComponent
{
private:
	// CONST VARS
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
		Throw,		// Ready to charge
		Charge,		// Charging
		Teleport,	// Ready to tp
		Wait		// Just teleported and can not throw
	};
private:
	TeleportState	m_tpState;
	float			m_charge;
	float			m_travelSpeed;
	bool			m_useFunctionCalled;
	PointLight		m_light;

	Quad * m_bar;
public:
	TeleportAbility(void * owner = nullptr);
	~TeleportAbility();

	void Init() override;

	/* This Function needs to be used before the Update() function */
	void Use() override;
	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;


	void Draw() override;

private:
	// Private functions
	void _logic(double deltaTime);
	void _updateLight();
};