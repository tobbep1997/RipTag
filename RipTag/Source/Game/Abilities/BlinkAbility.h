#pragma once
#include "AbilityComponent.h"

class BlinkAbility : public AbilityComponent
{
public:
	static const float RANGE;
	static const int MANA_COST = 10;
public:
	BlinkAbility();
	~BlinkAbility();

	void Init() override;

	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;
	void UpdateFromNetwork(Network::ENTITYABILITYPACKET * data) override;
	/* This Function needs to be used before the Update() function */
	void Use() override;

	void Draw() override;
private:
	enum BlinkState
	{
		Blink,	// Ready to blink
		Wait	// Just Blinked
	};
	BlinkState	m_bState;
	bool		m_useFunctionCalled;
	int m_rayId = -100;
	int m_rayId2 = -100;

private:
	void _logic(double deltaTime);
	void _sendBlinkPacket();
};

