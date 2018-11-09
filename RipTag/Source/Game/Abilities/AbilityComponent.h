#pragma once
#pragma warning (disable : 4244)
#include <NetworkMessageIdentifiers.h>

enum Ability
{
	TELEPORT = 0,
	VISIBILITY = 1,
	DISABLE = 2,
	VIS2 = 3, //this is a placeholder
	BLINK = 0,
	POSSESS = 2,
	NONE = 999
};

class AbilityComponent
{
protected: // CD
	float p_cooldownMax= 5.0f;
	float p_cooldown = 0;
private:
	float m_manaCost;
protected:
	void * p_owner;
	bool isLocal;

public:
	AbilityComponent(void * owner = nullptr);
	virtual ~AbilityComponent();

	virtual void setOwner(void * owner);
	virtual void setIsLocal(bool value);

	virtual void setManaCost(float mana);
	virtual float getManaCost() const;

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