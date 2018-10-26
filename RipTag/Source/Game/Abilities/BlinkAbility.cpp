#include "BlinkAbility.h"
#include "../Actors/Player.h"


BlinkAbility::BlinkAbility()
{
	m_bState = Blink;
	m_useFunctionCalled = false;
	this->m_rayListener = new RayCastListener();
}


BlinkAbility::~BlinkAbility()
{
	delete this->m_rayListener;
}

void BlinkAbility::Init()
{
}

void BlinkAbility::Update(double deltaTime)
{
	_logic(deltaTime);
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
			this->m_rayListener->shotRay(pPointer->getBody(), pPointer->getCamera()->getDirection(), RANGE);
			if ((int)this->m_rayListener->bodyUserData == 1)
			{
				pPointer->setPosition(
					this->m_rayListener->contactPoint.x + (
					(abs(this->m_rayListener->contactPoint.x - this->m_rayListener->shape->GetBody()->GetTransform().translation.x) * 2) *
						(-this->m_rayListener->normal.x)),
					pPointer->getPosition().y,
					this->m_rayListener->contactPoint.z + (
					(abs(this->m_rayListener->contactPoint.z - this->m_rayListener->shape->GetBody()->GetTransform().translation.z) * 2) *
						(-this->m_rayListener->normal.z))
				);
				if (this->m_rayListener->normal.y != 0)
				{
					pPointer->setPosition(
						pPointer->getPosition().x,
						this->m_rayListener->contactPoint.y + (
						(abs(this->m_rayListener->contactPoint.y - this->m_rayListener->shape->GetBody()->GetTransform().translation.y) * 2) *
							(-this->m_rayListener->normal.y)),
						pPointer->getPosition().z
					);
				}
				m_bState = BlinkState::Wait;
			}
			this->m_rayListener->clear();
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
