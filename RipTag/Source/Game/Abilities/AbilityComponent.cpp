#include "RipTagPCH.h"
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

void AbilityComponent::setMaxCooldown(const float & maxTime)
{
	this->p_cooldownMax = maxTime;
}

float AbilityComponent::getMaxCooldWon() const
{
	return this->p_cooldownMax;
}

float AbilityComponent::getPercentage() const
{
	return p_cooldown / p_cooldownMax;
}

void AbilityComponent::updateCooldown(double deltaTime)
{
	if (p_cooldown > 0)
		p_cooldown += deltaTime;
}
