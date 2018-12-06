#include "RipTagPCH.h"
#include "BlinkAbility.h"


BlinkAbility::BlinkAbility()
{
	m_bState = Blink;
	m_useFunctionCalled = false;
	
}


BlinkAbility::~BlinkAbility()
{
}

void BlinkAbility::Init()
{
}

void BlinkAbility::Update(double deltaTime)
{
	if (this->isLocal)
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

//Shoots a ray, gets the contact point and the normal, puts the player on the opposite side of the wall
void BlinkAbility::_logic(double deltaTime)
{
	if (RipExtern::g_rayListener->hasRayHit(m_rayId))
	{
		Player* pPointer = static_cast<Player*>(p_owner);
		RayCastListener::Ray* ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_rayId);
		RayCastListener::RayContact* var = ray->getClosestContact();
		if (ray->getOriginBody()->GetObjectTag() == "PLAYER" && var->contactShape->GetBody()->GetObjectTag() == "BLINK_WALL")
		{
			_sendBlinkPacket();
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("phase");
			pPointer->setPosition(
				var->contactPoint.x + (
				(fabs(fabs(var->contactPoint.x) - fabs(var->contactShape->GetBody()->GetTransform().translation.x)) * 2 + 0.25)*
					(-var->normal.x)),
				pPointer->getPosition().y,
				var->contactPoint.z + (
				(fabs(fabs(var->contactPoint.z) - fabs(var->contactShape->GetBody()->GetTransform().translation.z)) * 2 + 0.25) *
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

			m_bState = BlinkState::Wait;
		}

	}

	if (((Player *)p_owner)->getCurrentAbility() == Ability::BLINK && Input::OnAbilityPressed()) // the Use() function was called last frame
	{
		Player* pPointer = static_cast<Player*>(p_owner);
		switch (m_bState)
		{
		case BlinkState::Wait:
			p_cooldown += deltaTime;
			if (p_cooldown >= p_cooldownMax)
			{
				m_bState = BlinkState::Blink;
				p_cooldown = 0;
			}
			break;
		case BlinkState::Blink:
			if(m_rayId == -100)
				m_rayId = RipExtern::g_rayListener->PrepareRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), BlinkAbility::RANGE);
			break;
		}
	}
	else
	{	
		if(m_bState == BlinkState::Wait)
		{
			p_cooldown += deltaTime;
			if (p_cooldown >= p_cooldownMax)
			{
				m_bState = BlinkState::Blink;
				p_cooldown = 0;
			}
		}
	}
}

void BlinkAbility::_sendBlinkPacket()
{
	Network::COMMONEVENTPACKET packet(Network::ID_PLAYER_BLINK);
	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::IMMEDIATE_PRIORITY);
}
