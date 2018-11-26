#pragma once
#include "AbilityComponent.h"

class Enemy;

class PossessGuard : public AbilityComponent
{
private:
	const float RANGE = 10.0f;
	const float COOLDOWN_POSSESSING_MAX = 10.0f;
	const int MANA_COST_START = 5;
	const float MANA_COST_DRAIN = 1.0f;
	enum PossessState
	{
		Possess,	// Ready to possess
		Possessing,	// Is possessing
		Wait		// Just possessed
	};
	PossessState	m_pState;
	Enemy*			m_possessTarget;
	float			m_duration;
	bool			m_useFunctionCalled;
	Quad *			m_possessHud;
	int m_rayId = -100;
public:
	PossessGuard(void * owner = nullptr);
	~PossessGuard();

	void Init() override;

	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;
	void UpdateFromNetwork(Network::ENTITYABILITYPACKET * data) override;
	/* This Function needs to be used before the Update() function */
	void Use() override;

	void Draw() override;
private:

	void _logic(double deltaTime);
};

