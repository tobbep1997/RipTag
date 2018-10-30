#pragma once
#include "AbilityComponent.h"
#include "../../Physics/Wrapper/RayCastListener.h"

class BlinkAbility : public AbilityComponent
{
private:
	const float RANGE = 10.0f;
	const float COOLDOWN_WAIT_MAX = 1.0f;

	enum BlinkState
	{
		Blink,	// Ready to blink
		Wait	// Just Blinked
	};
	BlinkState	m_bState;
	RayCastListener *m_rayListener;
	float cooldown;
	bool m_useFunctionCalled;

public:
	BlinkAbility();
	~BlinkAbility();

	void Init() override;

	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;

	/* This Function needs to be used before the Update() function */
	void Use() override;

	void Draw() override;
private:
	void _logic(double deltaTime);
};
