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
	m_useFunctionCalled = false;
	if (Input::OnAbilityPressed())
		this->Use();
	Player* pPointer = static_cast<Player*>(p_owner);
	/*if (Input::OnAbilityReleased())
		m_useFunctionCalled = true;*/

	if (m_useFunctionCalled) // the Use() function were called last frame
	{
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
			if (!pPointer->IsInputLocked()) //Player is Returning to body
			{
				this->possessTarget->LockEnemyInput();
				pPointer->getBody()->SetType(e_dynamicBody);
				pPointer->getBody()->SetAwake(true);
				CameraHandler::setActiveCamera(pPointer->getCamera());
				this->possessTarget = nullptr;
				m_pState = PossessGuard::Wait;
				cooldown = 0;
				//m_useFunctionCalled = false;
			}
			else if (!pPointer->CheckManaCost(getManaCost())) //out of mana
			{
				this->possessTarget->removePossessor();
			}
			pPointer->DrainMana(MANA_COST_DRAIN*deltaTime);
			break;
		case PossessGuard::Possess:
			if (pPointer->CheckManaCost(getManaCost()))
			{
				RayCastListener::Ray* ray = RipExtern::m_rayListener->ShotRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), PossessGuard::RANGE, true);

				if (ray != nullptr)
				{
					RayCastListener::RayContact* contact = ray->getClosestContact();
					if (contact->originBody->GetObjectTag() == "PLAYER" && contact->contactShape->GetBody()->GetObjectTag() == "ENEMY")
					{
						pPointer->DrainMana(getManaCost());
						pPointer->getBody()->SetType(e_staticBody);
						pPointer->getBody()->SetAwake(false);
						pPointer->LockPlayerInput();

						this->possessTarget = static_cast<Enemy*>(contact->contactShape->GetBody()->GetUserData());
						contact->contactShape->GetBody()->SetType(e_dynamicBody);
						contact->contactShape->GetBody()->SetAwake(true);
						this->possessTarget->UnlockEnemyInput();
						this->possessTarget->setPossessor(pPointer, 20, 1);

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
			if (!pPointer->IsInputLocked()) //Player is Returning to body
			{
				this->possessTarget->LockEnemyInput();
				pPointer->getBody()->SetType(e_dynamicBody);
				pPointer->getBody()->SetAwake(true);
				CameraHandler::setActiveCamera(pPointer->getCamera());
				this->possessTarget = nullptr;
				m_pState = PossessGuard::Wait;
			}
			else if (!pPointer->CheckManaCost(getManaCost())) //out of mana
			{
				this->possessTarget->removePossessor();
			}
			pPointer->DrainMana(MANA_COST_DRAIN*deltaTime);
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
}
