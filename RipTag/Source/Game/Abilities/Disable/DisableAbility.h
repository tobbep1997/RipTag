#pragma once

#include <Multiplayer.h>

#include "../AbilityComponent.h"
#include "../../Actors/BaseActor.h"
#include "2D Engine/Quad/Components/HUDComponent.h"

class DisableAbility : public AbilityComponent, public HUDComponent
{
public:
	inline static const float SMOKE_LIFE = 1.5f;
private: // CONST VARS
	/*
	 * This will be the start mana cost. But the mana kan be changed
	 * This is beacuse abilitycomponent is the one holding mana. and not the component it self
	 */
	const float TRAVEL_SPEED = 20.0f;
	const float MAX_CHARGE = 1.0f;
	Circle * m_bar = nullptr;

private:
	// ENUM
	enum DisableState
	{
		Throwable,		// Ready to charge
		Charging,		// Charging
		Moving,
		Cooldown,
		RemoteActive,  //for network
		OnHit
	};

private:
	DisableState	m_dState;
	float			m_charge;
	float			m_travelSpeed;
	BaseActor*		m_obj;

	bool m_canceled = false;
	//Network
	DirectX::XMFLOAT4A m_lastStart;
	DirectX::XMFLOAT4A m_lastVelocity;
	RakNet::Time delay;

	bool m_hasHit = false; 
	bool m_isActive = false; 

public:
	DisableAbility(void * owner = nullptr);
	~DisableAbility();

	void Init() override;

	/* This Function needs to be used after the Use() function */
	void Update(double deltaTime) override;
	void UpdateFromNetwork(Network::ENTITYABILITYPACKET * data) override;

	bool getIsActive() const; 

	/* This Function needs to be used before the Update() function */
	void Use() override;

	void Draw() override;

	void Reset();

	DirectX::XMFLOAT4A getVelocity();
	DirectX::XMFLOAT4A getStart();
	unsigned int getState();

private:
	// Private functions
	void _logicLocal(double deltaTime, Camera* camera);
	void _logicRemote(double dt, Camera * camera);

	void _inStateThrowable();
	void _inStateCharging(double dt);
	void _inStateMoving(double dt);
	void _inStateCooldown(double dt);

	void _sendOnHitNotification(Enemy * ptr = nullptr);
	void _sendSmokeNotification();
};
