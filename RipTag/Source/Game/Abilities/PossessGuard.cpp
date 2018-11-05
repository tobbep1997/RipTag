#include "RipTagPCH.h"
#include "PossessGuard.h"



PossessGuard::PossessGuard(void * owner) : AbilityComponent(owner)
{
	m_pState = Possess;
	m_useFunctionCalled = false;
	setManaCost(MANA_COST_START);
}

PossessGuard::~PossessGuard()
{
	possessTarget = nullptr;
}

void PossessGuard::Init()
{
}

void PossessGuard::Update(double deltaTime)
{
	_logic(deltaTime);
}

void PossessGuard::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
}

void PossessGuard::Use()
{
	m_useFunctionCalled = true;
}

void PossessGuard::Draw()
{
}

void PossessGuard::_logic(double deltaTime)
{
	if (m_useFunctionCalled) // the Use() function were called last frame
	{
		Player* pPointer = static_cast<Player*>(p_owner);
		switch (m_pState)
		{
		case PossessGuard::Wait:
			if (cooldown >= COOLDOWN_WAIT_MAX)
			{
				m_pState = PossessGuard::Possess;
				cooldown = 0;
			}
			else
				cooldown += deltaTime;
			break;
		case PossessGuard::Possessing:
				this->possessTarget->LockEnemyInput();
				CameraHandler::setActiveCamera(static_cast<Player*>(p_owner)->getCamera());
				this->possessTarget = nullptr;
				m_pState = PossessGuard::Wait;
				cooldown = 0;
			
			break;
		case PossessGuard::Possess:
			if (((Player*)p_owner)->CheckManaCost(getManaCost()))
			{
				RayCastListener::RayContact* contact = RipExtern::m_rayListener->ShotRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), PossessGuard::RANGE, true);

				if (contact->contactShape != nullptr)
				{
					if (contact->originBody->GetObjectTag() == "PLAYER" &&
						contact->contactShape->GetBody()->GetObjectTag() == "ENEMY")
					{
						((Player*)p_owner)->DrainMana(getManaCost());
						this->possessTarget = static_cast<Enemy*>(contact->contactShape->GetBody()->GetUserData());
						contact->contactShape->GetBody()->SetType(e_dynamicBody);
						contact->contactShape->GetBody()->SetAwake(true);
						this->possessTarget->UnlockEnemyInput();
						this->possessTarget->setPossessor(pPointer, 20, 1);
						pPointer->LockPlayerInput();
						CameraHandler::setActiveCamera(this->possessTarget->getCamera());
						m_pState = PossessGuard::Possessing;
						cooldown = 0;
					}
				}		
			}
			break;
		}
	}
	else
	{
		switch (m_pState)
		{
		case PossessGuard::Possessing:
			if (!static_cast<Player*>(p_owner)->IsInputLocked()) //Player is Returning to body
			{
				this->possessTarget->LockEnemyInput();
				CameraHandler::setActiveCamera(static_cast<Player*>(p_owner)->getCamera());
				this->possessTarget = nullptr;
				m_pState = PossessGuard::Wait;
			}
			else if (!((Player*)p_owner)->CheckManaCost(getManaCost())) //out of mana
			{
				this->possessTarget->removePossessor();
			}
			((Player*)p_owner)->DrainMana(MANA_COST_DRAIN*deltaTime);
			break;
		case PossessGuard::Wait:
			if (cooldown >= COOLDOWN_WAIT_MAX)
			{
				m_pState = PossessGuard::Possess;
				cooldown = 0;
			}
			else
				cooldown += deltaTime;

			break;
		}
	
	}
	m_useFunctionCalled = false;
}
