#pragma once
#pragma warning (disable : 4244)
#include <NetworkMessageIdentifiers.h>

enum Ability
{
	TELEPORT = 0,
	DISABLE = 1,
	BLINK = 0,
	POSSESS = 1,
	NONE = 999
};

class AbilityComponent
{
protected: // CD
	float p_cooldownMax= 8.0f;
	float p_cooldown = 0;

protected:
	void * p_owner;
	bool isLocal;
	bool p_wasLocked = false; 

public:
	AbilityComponent(void * owner = nullptr);
	virtual ~AbilityComponent();

	virtual void setOwner(void * owner);
	virtual void setIsLocal(bool value);

	virtual void Init() = 0;
	virtual void Update(double deltaTime) = 0;
	virtual void UpdateFromNetwork(Network::ENTITYABILITYPACKET * data) = 0;
	virtual void Use() = 0;
	virtual void Draw() = 0;

	virtual void setMaxCooldown(const float & maxTime);
	virtual float getMaxCooldWon() const;

	virtual float getPercentage() const;

	virtual void updateCooldown(double deltaTime);
};