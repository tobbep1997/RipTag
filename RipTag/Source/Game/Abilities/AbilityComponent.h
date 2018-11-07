#pragma once
#pragma warning (disable : 4244)
#include <NetworkMessageIdentifiers.h>

enum Ability
{
	TELEPORT = 0,
	VISIBILITY = 1,
	DISABLE = 2,
	VIS2 = 3, //this is a placeholder
	BLINK = 4,
	POSSESS = 5,
};

class AbilityComponent
{
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
};