#include "RipTagPCH.h"
#include "BlinkAbility.h"


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
			if (pPointer->CheckManaCost(getManaCost()))
			{
				RayCastListener::RayContact* var = RipExtern::m_rayListener->ShotRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), BlinkAbility::RANGE, true);
			
				if(var != nullptr)
				{
					if(var->originBody->GetObjectTag() == "PLAYER" && var->contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
					{
						pPointer->setPosition(
							var->contactPoint.x + (
							(fabs(fabs(var->contactPoint.x) - fabs(var->contactShape->GetBody()->GetTransform().translation.x)) *2 + 0.25)*
								(-var->normal.x)),
							pPointer->getPosition().y,
							var->contactPoint.z + (
							(fabs(fabs(var->contactPoint.z) - fabs(var->contactShape->GetBody()->GetTransform().translation.z))*2 + 0.25) *
								(-var->normal.z))
						);
						if (fabs(var->normal.y) > 0.001)
						{
							pPointer->setPosition(
								pPointer->getPosition().x,
								var->contactPoint.y + (
								(fabs(var->contactPoint.y - (var->contactShape->GetBody()->GetTransform().translation.y) * 2)) *
									(-var->normal.y)),
								pPointer->getPosition().z
							);
						}
						std::cout << var->originBody->GetTransform().translation.x << " " <<
							var->originBody->GetTransform().translation.y << " " <<
							var->originBody->GetTransform().translation.z << " " << std::endl << std::endl;
						pPointer->DrainMana(getManaCost());
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
