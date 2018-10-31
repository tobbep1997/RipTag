#pragma once
#include "AbilityComponent.h"
#include "../../Physics/Wrapper/RayCastListener.h"
#include "../Actors/Enemy/Enemy.h"

class PossessGuard : public AbilityComponent
{
private:
	const float RANGE = 10.0f;
	const float COOLDOWN_WAIT_MAX = 1.0f;
	const float COOLDOWN_POSSESSING_MAX = 1.0f;
	const int MANA_COST_START = 5.0f;
	const float MANA_COST_TICK_RATE = 1.0f;
	enum PossessState
	{
		Possess,	// Ready to possess
		Possessing,	// Is possessing
		Wait		// Just possessed
	};
	PossessState	m_pState;
	RayCastListener *m_rayListener;
	Enemy* possessTarget;
	float cooldown;
	bool m_useFunctionCalled;
public:
	PossessGuard(void * owner = nullptr);
	~PossessGuard();

	void Init() override;

	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;

	/* This Function needs to be used before the Update() function */
	void Use() override;

	void Draw() override;
private:

	void _logic(double deltaTime);
};

