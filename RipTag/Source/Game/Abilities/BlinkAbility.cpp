#include "BlinkAbility.h"
#include "../Actors/Player.h"
#include "../../../RipTagExtern/RipExtern.h"

BlinkAbility::BlinkAbility()
{
	m_bState = Blink;
	m_useFunctionCalled = false;
	setManaCost(MANA_COST);
}


BlinkAbility::~BlinkAbility()
{
}

void BlinkAbility::Init()
{
}

void BlinkAbility::Update(double deltaTime)
{
	_logic(deltaTime);
}

void BlinkAbility::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
}

void BlinkAbility::Use()
{
	m_useFunctionCalled = true;
}

void BlinkAbility::Draw()
{
}

void BlinkAbility::_logic(double deltaTime)
{
	if (m_useFunctionCalled) // the Use() function were called last frame
	{
		Player* pPointer = static_cast<Player*>(p_owner);
		switch (m_bState)
		{
		case BlinkState::Wait:
			if (cooldown >= COOLDOWN_WAIT_MAX)
			{
				m_bState = BlinkState::Blink;
				cooldown = 0;
			}
			else
				cooldown += deltaTime;
			break;
		case BlinkState::Blink:
			if (((Player*)p_owner)->CheckManaCost(getManaCost()))
			{
				RayCastListener::RayContact var = RipExtern::m_rayListener->ShotRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), BlinkAbility::RANGE, true);
			
				if(var.originBody->GetObjectTag() == "PLAYER")
				{
					//std::cout << var.contactPoint.x << " " << var.contactPoint.y << " " << var.contactPoint.z << " " << std::endl;
					if(var.contactShape != nullptr && var.contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
					{
						pPointer->setPosition(
							var.contactPoint.x + (
							(abs(var.contactPoint.x - (var.contactShape->GetBody()->GetTransform().translation.x) * 2) +0.25) *
								(-var.normal.x)),
							pPointer->getPosition().y,
							var.contactPoint.z + (
							(abs(var.contactPoint.z - (var.contactShape->GetBody()->GetTransform().translation.z) * 2) +0.25) *
								(-var.normal.z))
						);
						if (abs(var.normal.y) > 0.001)
						{
							pPointer->setPosition(
								pPointer->getPosition().x,
								var.contactPoint.y + (
								(abs(var.contactPoint.y - (var.contactShape->GetBody()->GetTransform().translation.y) * 2)) *
									(-var.normal.y)),
								pPointer->getPosition().z
							);
						}
						((Player*)p_owner)->DrainMana(getManaCost());
						m_bState = BlinkState::Wait;
					}
				}
			}
			break;
		}
	}
	else
	{
		switch (m_bState)
		{
		case BlinkState::Blink:

			break;
		case BlinkState::Wait:
			if (cooldown >= COOLDOWN_WAIT_MAX)
			{
				m_bState = BlinkState::Blink;
				cooldown = 0;
			}
			else
				cooldown += deltaTime;
			break;
		}

	}
	m_useFunctionCalled = false;
}
