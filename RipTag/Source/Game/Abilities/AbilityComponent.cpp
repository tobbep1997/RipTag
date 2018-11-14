#include "RipTagPCH.h"
#include "AbilityComponent.h"

AbilityComponent::AbilityComponent(void * owner)
{
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
