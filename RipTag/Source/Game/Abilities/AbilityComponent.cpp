#include "AbilityComponent.h"

AbilityComponent::AbilityComponent(void * owner)
{
	m_manaCost = 0;
	p_owner = owner;
}

AbilityComponent::~AbilityComponent()
{
}

void AbilityComponent::setOwner(void * owner)
{
	p_owner = owner;
}

void AbilityComponent::setIsLocal(bool value)
{
	isLocal = value;
}

void AbilityComponent::setManaCost(float mana)
{
	m_manaCost = mana;
}

float AbilityComponent::getManaCost() const
{
	return m_manaCost;
}