#include "RipTagPCH.h"
#include "PossessGuard.h"



PossessGuard::PossessGuard(void * owner) : AbilityComponent(owner)
{
	m_pState = Possess;
	m_useFunctionCalled = false;
	
	m_possessHud = new Quad();
	m_possessHud->init({ 0.5f, 1.0f }, { 0.4f, 0.2f });
	m_possessHud->setPivotPoint(Quad::PivotPoint::upperCenter);
	m_possessHud->setFont(FontHandler::getFont("consolas32"));
	m_possessHud->setUnpressedTexture("SPHERE");
	m_possessHud->setPressedTexture("DAB");
	m_possessHud->setHoverTexture("PIRASRUM");
	m_possessHud->setTextColor(DirectX::XMFLOAT4A(1, 1, 1, 1));
}

PossessGuard::~PossessGuard()
{
	m_possessTarget = nullptr;
	m_possessHud->Release();
	delete m_possessHud;
}

void PossessGuard::Init()
{
}

void PossessGuard::Update(double deltaTime)
{
	if (this->isLocal)
		_logic(deltaTime);
}

void PossessGuard::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
}

void PossessGuard::Use()
{
}

void PossessGuard::Draw()
{
	if (PossessGuard::Possessing == m_pState)
		m_possessHud->Draw();
}

void PossessGuard::_logic(double deltaTime)
{
	switch (m_pState)
	{
		case PossessState::Possess:
		{
			if (Input::OnAbility2Pressed())
			{
				_hitEnemy();
			}
		}
		break;

		case PossessState::Possessing:
		{
			_isPossessing(deltaTime);
		}
		break;

		case PossessState::Wait:
		{
			_onCooldown(deltaTime);
		}
		break;
	}

	//if (true) // the Use() function were called last frame
	//{
	//	switch (m_pState)
	//	{
	//	case PossessGuard::Wait:
	//		p_cooldown += deltaTime;
	//		if (p_cooldown >= p_cooldownMax)
	//		{
	//			m_pState = PossessGuard::Possess;
	//			p_cooldown = 0;
	//		}
	//		break;
	//	case PossessGuard::Possessing:
	//		if (!pPointer->IsInputLocked()) //Player is Returning to body
	//		{
	//			this->_sendOverNetwork(false, m_possessTarget);

	//			this->m_possessTarget->LockEnemyInput();
	//			pPointer->getBody()->SetType(e_dynamicBody);
	//			pPointer->getBody()->SetAwake(true);
	//			pPointer->setHidden(true);
	//			CameraHandler::setActiveCamera(pPointer->getCamera());
	//			this->m_possessTarget->setKnockOutType(this->m_possessTarget->Possessed);
	//			this->m_possessTarget->setTransitionState(AITransitionState::ExitingPossess);
	//			this->m_possessTarget = nullptr;
	//			m_pState = PossessGuard::Wait;
	//			p_cooldown = 0; 
	//			m_duration = 0;
	//			//m_useFunctionCalled = false;

	//		}
	//		else if (m_duration >= COOLDOWN_POSSESSING_MAX) //out of mana
	//		{
	//			this->m_possessTarget->removePossessor();
	//			m_duration = 0;
	//		}
	//		else
	//		{
	//			float p = ((COOLDOWN_POSSESSING_MAX - m_duration) / COOLDOWN_POSSESSING_MAX);
	//			m_possessHud->setScale(0.4f * p, 0.2);
	//			std::string str = std::to_string(p * COOLDOWN_POSSESSING_MAX);
	//			m_possessHud->setString(str.substr(0, 4));
	//		}

	//		
	//		break;
	//	case PossessGuard::Possess:
	//		if (RipExtern::g_rayListener->hasRayHit(m_rayId) && m_useFunctionCalled)
	//		{
	//			RayCastListener::Ray *ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_rayId);
	//			RayCastListener::RayContact* contact = ray->getClosestContact();
	//			if (ray->getOriginBody()->GetObjectTag() == "PLAYER")
	//			{
	//				std::string objectTag = contact->contactShape->GetBody()->GetObjectTag();
	//				if (objectTag == "BLINK_WALL")
	//				{
	//					if (ray->getNrOfContacts() > 1)
	//					{
	//						contact = ray->GetRayContacts().at(ray->getNrOfContacts() - 2);
	//						objectTag = contact->contactShape->GetBody()->GetObjectTag();
	//					}
	//				}

	//				if (objectTag == "ENEMY")
	//				{
	//					pPointer->getBody()->SetType(e_staticBody);
	//					pPointer->getBody()->SetAwake(false);
	//					pPointer->setHidden(false);
	//					pPointer->LockPlayerInput();

	//					contact->contactShape->GetBody()->SetType(e_dynamicBody);
	//					contact->contactShape->GetBody()->SetAwake(true);
	//					this->m_possessTarget = static_cast<Enemy*>(contact->contactShape->GetBody()->GetUserData());
	//					this->m_possessTarget->setTransitionState(AITransitionState::BeingPossessed);
	//					this->m_possessTarget->setPossessor(pPointer, 20, 1);
	//					m_pState = PossessGuard::Possessing;
	//					p_cooldown = 0;
	//					//m_possessHud->setScale(1.0f / COOLDOWN_POSSESSING_MAX, 0.2);
	//					this->_sendOverNetwork(true, m_possessTarget);
	//				}
	//			}
	//		}
	//		if(m_rayId == -100 && m_pState == PossessGuard::Possess)
	//			m_rayId = RipExtern::g_rayListener->PrepareRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), PossessGuard::RANGE);
	//		
	//		break;
	//	}
	//}
	//else
	//{
	//	switch (m_pState)
	//	{
	//	case PossessGuard::Possessing:
	//		if (!pPointer->IsInputLocked()) //Player is Returning to body
	//		{
	//			this->m_possessTarget->LockEnemyInput();
	//			pPointer->getBody()->SetType(e_dynamicBody);
	//			pPointer->getBody()->SetAwake(true);
	//			pPointer->setHidden(true);
	//			CameraHandler::setActiveCamera(pPointer->getCamera());
	//			this->m_possessTarget->setTransitionState(AITransitionState::ExitingPossess);
	//			this->m_possessTarget = nullptr;
	//			m_pState = PossessGuard::Wait;
	//			p_cooldown = 0;
	//			m_duration = 0;
	//			//m_useFunctionCalled = false;
	//		}
	//		else if (m_duration >= COOLDOWN_POSSESSING_MAX) //out of mana
	//		{
	//			this->m_possessTarget->removePossessor();
	//			m_duration = 0;
	//		}
	//		else
	//		{
	//			m_duration += deltaTime;
	//			float p = ((COOLDOWN_POSSESSING_MAX - m_duration) / COOLDOWN_POSSESSING_MAX);
	//			m_possessHud->setScale(0.4f * p , 0.2);
	//			std::string str = std::to_string(p * COOLDOWN_POSSESSING_MAX);
	//			m_possessHud->setString(str.substr(0, 4));
	//		}
	//		
	//		break;
	//	case PossessGuard::Wait:
	//		p_cooldown += deltaTime;
	//		if (p_cooldown >= p_cooldownMax)
	//		{
	//			m_pState = PossessGuard::Possess;
	//			p_cooldown = 0;
	//		}

	//		break;
	//	}
	//
	//}
}

void PossessGuard::_sendOverNetwork(bool state, Enemy * ptr)
{
	if (Network::Multiplayer::GetInstance()->isConnected())
	{
		Network::ENTITYSTATEPACKET packet(0, 0, 0);
		packet.id = Network::ID_ENEMY_POSSESSED;
		packet.condition = state;
		packet.state = ptr->getUniqueID();

		Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
	}
}

void PossessGuard::_hitEnemy()
{
	Player* pPointer = static_cast<Player*>(p_owner);

	if (RipExtern::g_rayListener->hasRayHit(m_rayId))
	{
		RayCastListener::Ray *ray = RipExtern::g_rayListener->ConsumeProcessedRay(m_rayId);
		RayCastListener::RayContact* contact = ray->getClosestContact();
		if (ray->getOriginBody()->GetObjectTag() == "PLAYER")
		{
			std::string objectTag = contact->contactShape->GetBody()->GetObjectTag();
			if (objectTag == "BLINK_WALL")
			{
				if (ray->getNrOfContacts() > 1)
				{
					contact = ray->GetRayContacts().at(ray->getNrOfContacts() - 2);
					objectTag = contact->contactShape->GetBody()->GetObjectTag();
				}
			}


			if (objectTag == "ENEMY")
			{
				Enemy * e = static_cast<Enemy*>(contact->contactShape->GetBody()->GetUserData());
				if (e->getAIState() != AIState::Disabled)
				{
					pPointer->getBody()->SetType(e_staticBody);
					pPointer->getBody()->SetAwake(false);
					pPointer->setHidden(false);
					pPointer->LockPlayerInput();

					contact->contactShape->GetBody()->SetType(e_dynamicBody);
					contact->contactShape->GetBody()->SetAwake(true);
					this->m_possessTarget = e;
					this->m_possessTarget->setTransitionState(AITransitionState::BeingPossessed);
					this->m_possessTarget->setPossessor(pPointer, 20, 1);
					m_pState = PossessGuard::Possessing;
					p_cooldown = 0;
					//m_possessHud->setScale(1.0f / COOLDOWN_POSSESSING_MAX, 0.2);
					this->_sendOverNetwork(true, m_possessTarget);
				}
			}
		}
	}
	if (m_rayId == -100 && m_pState == PossessGuard::Possess)
		m_rayId = RipExtern::g_rayListener->PrepareRay(pPointer->getBody(), pPointer->getCamera()->getPosition(), pPointer->getCamera()->getDirection(), PossessGuard::RANGE);
}

void PossessGuard::_isPossessing(double dt)
{
	Player* pPointer = static_cast<Player*>(p_owner);

	if (!pPointer->IsInputLocked() || Input::OnCancelAbilityPressed()) //Player is Returning to body
	{
		this->_sendOverNetwork(false, m_possessTarget);

		pPointer->getBody()->SetType(e_dynamicBody);
		pPointer->getBody()->SetAwake(true);
		pPointer->setHidden(true);

		CameraHandler::setActiveCamera(pPointer->getCamera());

		this->m_possessTarget->LockEnemyInput();
		this->m_possessTarget->setKnockOutType(this->m_possessTarget->Possessed);
		this->m_possessTarget->setTransitionState(AITransitionState::ExitingPossess);
		this->m_possessTarget = nullptr;

		m_pState = PossessGuard::Wait;
		p_cooldown = 0;
		m_duration = 0;
	}
	else if (m_duration >= COOLDOWN_POSSESSING_MAX) //out of mana
	{
		this->m_possessTarget->removePossessor();
		m_duration = 0;
	}
	else
	{
		m_duration += dt;
		float p = ((COOLDOWN_POSSESSING_MAX - m_duration) / COOLDOWN_POSSESSING_MAX);
		m_possessHud->setScale(0.4f * p , 0.2);
		std::string str = std::to_string(p * COOLDOWN_POSSESSING_MAX);
		m_possessHud->setString(str.substr(0, 4));
	}
}

void PossessGuard::_onCooldown(double dt)
{
	p_cooldown += dt;
	if (p_cooldown >= p_cooldownMax)
	{
		m_pState = PossessGuard::Possess;
		p_cooldown = 0;
	}
}
