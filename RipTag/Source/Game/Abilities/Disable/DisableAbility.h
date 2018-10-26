#pragma once

#pragma once
#include "../AbilityComponent.h"
#include "../../Actors/BaseActor.h"
#include "../../../../RipTagExtern/RipExtern.h"

class DisableAbility : public AbilityComponent, public BaseActor
{
private:
	// CONST VARS
	const float TRAVEL_SPEED = 20.0f;
	const float MAX_CHARGE = 2.0f;
private:
	// ENUM
	enum DisableState
	{
		Throw,		// Ready to charge
		Charge,		// Charging
		Moving,
		Wait		// Just teleported and can not throw
	};
private:
	DisableState	m_dState;
	float			m_charge;
	float			m_travelSpeed;
	bool			m_useFunctionCalled;
public:
	DisableAbility(void * owner = nullptr);
	~DisableAbility();

	void Init() override;

	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;

	/* This Function needs to be used before the Update() function */
	void Use() override;

	void Draw() override;

private:
	// Private functions
	void _logic(double deltaTime);

};
