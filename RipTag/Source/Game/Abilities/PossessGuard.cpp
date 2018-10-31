#include "PossessGuard.h"
#include "../Actors/Player.h"
#include "../Handlers/CameraHandler.h"

PossessGuard::PossessGuard(void * owner) : AbilityComponent(owner)
{
	m_pState = Possess;
	m_useFunctionCalled = false;
	this->m_rayListener = new RayCastListener();
}

PossessGuard::~PossessGuard()
{
	possessTarget = nullptr;
	delete this->m_rayListener;
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

int PossessGuard::getPossessState()
{
	return m_pState;
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
			if (cooldown >= COOLDOWN_POSSESSING_MAX)
			{
				m_pState = PossessGuard::Return;
				cooldown = 0;
			}
			else
				cooldown += deltaTime;
			break;
		case PossessGuard::Possess:

			if (this->m_rayListener->shotRay(pPointer->getBody(), pPointer->getCamera()->getDirection(), PossessGuard::RANGE))
			{
				if (this->m_rayListener->shape->GetBody()->GetObjectTag() == "Enemy")
				{
					this->possessTarget = static_cast<Enemy*>(this->m_rayListener->bodyUserData);
					this->possessTarget->UnlockEnemyInput();
					this->possessTarget->setPossessor(pPointer);
					pPointer->LockPlayerInput();
					CameraHandler::setActiveCamera(this->possessTarget->getCamera());
					m_pState = PossessGuard::Possessing;
				}
				this->m_rayListener->clear();
			}
			break;
		case PossessGuard::Return:
			if (!static_cast<Player*>(p_owner)->IsInputLocked())
			{
				this->possessTarget->LockEnemyInput();
				//static_cast<Player*>(p_owner)->UnlockPlayerInput();
				CameraHandler::setActiveCamera(static_cast<Player*>(p_owner)->getCamera());
				this->possessTarget = nullptr;
				m_pState = PossessGuard::Wait;
			}
			break;
		}
	}
	else
	{
		switch (m_pState)
		{
		case PossessGuard::Possessing:
			if (cooldown >= COOLDOWN_POSSESSING_MAX)
			{
				m_pState = PossessGuard::Return;
				cooldown = 0;
			}
			else
				cooldown += deltaTime;
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
		case PossessGuard::Return:	
			if (!static_cast<Player*>(p_owner)->IsInputLocked())
			{
				this->possessTarget->LockEnemyInput();
				CameraHandler::setActiveCamera(static_cast<Player*>(p_owner)->getCamera());
				this->possessTarget = nullptr;
				m_pState = PossessGuard::Wait;
			}
			break;
		}
	
	}
	m_useFunctionCalled = false;
}
