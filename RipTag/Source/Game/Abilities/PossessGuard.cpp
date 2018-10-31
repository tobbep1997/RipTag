#include "PossessGuard.h"
#include "../Actors/Player.h"
#include "../Handlers/CameraHandler.h"
#include "../../../RipTagExtern/RipExtern.h"

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
				RipExtern::m_rayListener->ShotRay(pPointer->getBody(), pPointer->getCamera()->getDirection(), PossessGuard::RANGE, "Enemy");

				for (RayCastListener::RayContact con : RipExtern::m_rayListener->GetContacts())
				{
					if (con.originBody->GetObjectTag() == "PLAYER" &&
						con.contactShape->GetBody()->GetObjectTag() == "Enemy")
					{
						((Player*)p_owner)->DrainMana(getManaCost());
						this->possessTarget = static_cast<Enemy*>(con.contactShape->GetBody()->GetUserData());
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
			else if(cooldown >= MANA_COST_TICK_RATE)
			{
				((Player*)p_owner)->DrainMana(getManaCost());
				cooldown = 0;
			}
			cooldown +=deltaTime;
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
