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
				RipExtern::m_rayListener->ShotRay(pPointer->getBody(), pPointer->getCamera()->getDirection(), BlinkAbility::RANGE, "BLINK_WALL");
				for (RayCastListener::RayContact con : RipExtern::m_rayListener->GetContacts())
				{
					if(con.originBody->GetObjectTag() == "PLAYER")
					{
						if(con.contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
						{
							pPointer->setPosition(
								con.contactPoint.x + (
								(abs(con.contactPoint.x - con.contactShape->GetBody()->GetTransform().translation.x) * 2) *
									(-con.normal.x)),
								pPointer->getPosition().y,
								con.contactPoint.z + (
								(abs(con.contactPoint.z - con.contactShape->GetBody()->GetTransform().translation.z) * 2) *
									(-con.normal.z))
							);
							if (con.normal.y != 0)
							{
								pPointer->setPosition(
									pPointer->getPosition().x,
									con.contactPoint.y + (
									(abs(con.contactPoint.y - con.contactShape->GetBody()->GetTransform().translation.y) * 2) *
										(-con.normal.y)),
									pPointer->getPosition().z
								);
							}
							((Player*)p_owner)->DrainMana(getManaCost());
							m_bState = BlinkState::Wait;
						}
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
