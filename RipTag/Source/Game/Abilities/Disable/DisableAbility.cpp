#include "RipTagPCH.h"
#include "DisableAbility.h"

DisableAbility::DisableAbility(void* owner) : AbilityComponent(owner)
{
	m_dState = Throwable;
	m_charge = 0.0f;
	m_travelSpeed = MAX_CHARGE;
}

DisableAbility::~DisableAbility()
{
	m_obj->PhysicsComponent::Release(*RipExtern::g_world);
	delete m_obj;
	this->deleteEffects(); 
	delete m_particleEmitters;
	m_bar->Release();
	delete m_bar;
}

void DisableAbility::Init()
{
	m_obj = DBG_NEW BaseActor();
	m_obj->PhysicsComponent::Init(*RipExtern::g_world, e_dynamicBody, 0.1f, 0.1f, 0.1f, false, 0.2f);
	m_obj->Drawable::setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	m_obj->Drawable::setScale(0.1f, 0.1f, 0.1f);
	m_obj->Drawable::setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	m_obj->BaseActor::setGravityScale(0.01f);
	m_obj->Transform::setPosition(-999.0f, -999.0f, -999.0f);


	m_obj->Drawable::setOutline(false);
	m_obj->Drawable::setOutlineColor(DirectX::XMFLOAT4A(1, 0, 0, 1));

	m_obj->PhysicsComponent::setUserDataBody(this);
	m_obj->getBody()->SetObjectTag("Disable");
	m_obj->getBody()->AddToFilters("PLAYER");

	m_bar = new Circle();
	Manager::g_textureManager.loadTextures("SPHERE");
	m_bar->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(1.0f / 16.0f, 1.0f / 9.0f));
	m_bar->setUnpressedTexture("SPHERE");
	m_bar->setPivotPoint(Quad::PivotPoint::center);
	m_bar->setRadie(.5f);
	m_bar->setInnerRadie(.4);
	m_bar->setAngle(0);

	p_abilityChargesMax = 2;
	p_abilityCharges = p_abilityChargesMax;

	m_particleEmitters = DBG_NEW ParticleEmitter*[p_abilityChargesMax];
	for (int i = 0; i < p_abilityChargesMax; i++)
	{
		m_particleEmitters[i] = nullptr;
	}
}

void DisableAbility::deleteEffects()
{
	for (int i = 0; i < p_abilityChargesMax; i++)
	{
		if (m_particleEmitters[i] != nullptr)
		{
			delete m_particleEmitters[i];
			m_particleEmitters[i] = nullptr;
		}
	}
}

ParticleEmitter ** DisableAbility::getEmitters()
{
	return m_particleEmitters; 
}

void DisableAbility::Update(double deltaTime)
{
	if (m_dState == DisableState::Moving || m_dState == DisableState::RemoteActive)
		m_obj->BaseActor::Update(deltaTime);
	Camera * camera = CameraHandler::getActiveCamera();
	if (this->isLocal)
		_logicLocal(deltaTime, camera);
	else
		_logicRemote(deltaTime, camera);
}

void DisableAbility::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
	switch (m_dState)
	{
	case DisableState::Throwable:
		if ((DisableState)data->state == DisableState::Moving)
		{
			DirectX::XMFLOAT4A pos = data->start;

			//adjust the start position based on the packets delay
			delay = RakNet::GetTime() - data->timeStamp;
			delay *= 0.001; //delay is in ms but we are using seconds

			pos.x += data->velocity.x * delay;
			pos.y += data->velocity.y * delay;
			pos.z += data->velocity.z * delay;

			m_obj->setPosition(pos.x, pos.y, pos.z);
			m_obj->setLiniearVelocity(data->velocity.x, data->velocity.y, data->velocity.z);
			
			this->m_dState = DisableState::RemoteActive;
		}
		break;
	case DisableState::RemoteActive:
		if ((DisableState)data->state == DisableState::OnHit)
		{
			//deleteEffects(); 

			m_obj->setPosition(-999.9f, -999.9f, -999.9f);
			m_obj->setLiniearVelocity(0.0f, 0.0f, 0.0f);
			this->m_dState = DisableState::Throwable;

			for (int i = 0; i < p_abilityChargesMax; i++)
			{
				if (!m_particleEmitters[i])
				{
					m_particleEmitters[i] = DBG_NEW ParticleEmitter();
					m_particleEmitters[i]->setSmoke();
					m_particleEmitters[i]->setEmmiterLife(1.5f);
					m_particleEmitters[i]->setPosition(data->start.x, data->start.y + 0.5f, data->start.z);
					break;
				}
			}
		}
		break;
	}
}

bool DisableAbility::getIsActive() const
{
	return m_isActive; 
}

void DisableAbility::Use()
{
}

void DisableAbility::Draw()
{
	if (m_dState == DisableState::Charging)
		m_bar->Draw();
	if (m_dState == DisableAbility::Moving ||m_dState == DisableAbility::RemoteActive)
	{
		m_obj->BaseActor::Draw();
	}
	for (int i = 0; i < p_abilityChargesMax; i++)
	{
		if (m_particleEmitters[i] != nullptr)
			m_particleEmitters[i]->Queue();

	}
}

DirectX::XMFLOAT4A DisableAbility::getVelocity()
{
	return this->m_lastVelocity;
}

DirectX::XMFLOAT4A DisableAbility::getStart()
{
	return this->m_lastStart;
}

unsigned int DisableAbility::getState()
{
	return (unsigned int)this->m_dState;
}

void DisableAbility::_logicLocal(double deltaTime, Camera* camera)
{
	switch (m_dState)
	{

	case DisableState::Throwable:
		this->_inStateThrowable();
		break;
	case DisableState::Charging:
		this->_inStateCharging(deltaTime);
		break;
	case DisableState::Moving:
		this->_inStateMoving(deltaTime);
		break;
	case DisableState::Cooldown:
		this->_inStateCooldown(deltaTime);
		break;
	}

	if (m_dState != DisableState::Cooldown && p_abilityCharges < p_abilityChargesMax)
	{
		p_cooldown += deltaTime;
		if (p_cooldown >= p_cooldownMax)
		{
			p_abilityCharges++;
			p_cooldown = 0;
		}
	}

	for (int i = 0; i < p_abilityChargesMax; i++)
	{
		if (m_particleEmitters[i] != nullptr)
		{
			m_particleEmitters[i]->Update(deltaTime, camera);
		}
	}

}

void DisableAbility::_logicRemote(double dt, Camera * camera)
{
	if (m_dState == DisableAbility::RemoteActive)
		this->_inStateRemoteActive(dt);

	for (int i = 0; i < p_abilityChargesMax; i++)
	{
		if (m_particleEmitters[i] != nullptr)
		{
			m_particleEmitters[i]->Update(dt, camera);
		}
	}
}

void DisableAbility::_inStateThrowable()
{
	using namespace Network;
	if (isLocal)
	{
		if (!m_canceled && ((Player *)p_owner)->getCurrentAbility() == Ability::DISABLE && Input::OnAbility2Pressed())
		{

			if (Multiplayer::GetInstance()->isConnected())
			{
				Network::COMMONEVENTPACKET packet(Network::NETWORKMESSAGES::ID_PLAYER_THROW_BEGIN);
				Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
			}

			m_dState = DisableAbility::Charging;
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("throw_ready");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->PopLayer("bob");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->PopLayer("turn");
		}
		if (Input::OnAbility2Released())
		{
			m_canceled = false;
		}
	}
}

void DisableAbility::_inStateCharging(double dt)
{
	using namespace Network;
	if (isLocal)
	{
		if (((Player *)p_owner)->getCurrentAbility() == Ability::DISABLE && Input::OnAbility2Pressed())
		{
			float charge = (m_charge / MAX_CHARGE);
			if (charge >= 1.0f)
				charge = 1.0f;
			m_bar->setAngle(360.0f * charge);
			//m_bar->setScale(1.0f *(m_charge / MAX_CHARGE), .1f);
			if (m_charge < MAX_CHARGE)
				m_charge += dt;
		}
		if (Input::OnCancelAbilityPressed())
		{
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("idle");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("bob");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("turn");
			m_charge = 0.0;
			m_dState = DisableState::Throwable;
			m_canceled = true;
		}
		if (Input::OnAbility2Released())
		{
			m_dState = DisableState::Moving;
			p_abilityCharges--;
			if (Multiplayer::GetInstance()->isConnected())
			{
				Network::COMMONEVENTPACKET packet(Network::NETWORKMESSAGES::ID_PLAYER_THROW_END);
				Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
			}

			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("throw_throw");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("bob");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("turn");
			DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
			DirectX::XMFLOAT4A start = XMMATH::add(((Player*)p_owner)->getCamera()->getPosition(), direction);
			this->m_lastStart = start;

			start.w = 1.0f;
			direction = XMMATH::scale(direction, TRAVEL_SPEED);
			m_obj->setPosition(start.x, start.y, start.z);
			m_obj->setLiniearVelocity(direction.x, direction.y, direction.z);
			m_lastVelocity = direction;
			m_charge = 0.0f;
		}
		else if (((Player *)p_owner)->getCurrentAbility() != Ability::DISABLE)
		{
			m_charge = 0.0f;
			m_dState = DisableState::Throwable;
		}
	}
}

void DisableAbility::_inStateMoving(double dt)
{
	//static double accumulatedTime = 0;
	static const double lifeDuration = 1.0 / 0.2; //5000 ms
	//accumulatedTime += dt;
	//p_cooldown = accumulatedTime;
	ContactListener::S_Contact contact;
	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
	{
		contact = RipExtern::g_contactListener->GetBeginContact(i);
		if (contact.a->GetBody()->GetObjectTag() == "Disable")
		{
			//Particle effects here before changing the position. 
			for (int j = 0; j < p_abilityChargesMax; j++)
			{
				if (!m_particleEmitters[j])
				{
					m_particleEmitters[j] = DBG_NEW ParticleEmitter();
					m_particleEmitters[j]->setSmoke();
					m_particleEmitters[j]->setEmmiterLife(1.5f);
					m_particleEmitters[j]->setPosition(m_obj->getPosition().x, m_obj->getPosition().y + 0.5f, m_obj->getPosition().z);
					break;
				}
			}
				
			m_dState = DisableState::Cooldown;
			m_obj->setPosition(-999.9f, -999.9f, -999.9f);

			//accumulatedTime = 0.0;
			m_isActive = true;

			if (contact.b->GetBody()->GetObjectTag() == "ENEMY")
			{
				Enemy * ptr = static_cast<Enemy*>(contact.b->GetBody()->GetUserData());
				if (ptr)
				{
					if (ptr->getAIState() != AIState::Possessed && !ptr->ClientLocked())
					{
						ptr->setTransitionState(AITransitionState::BeingDisabled); 
						this->_sendOnHitNotification(ptr);
					}
				}
			}
		}
	}

	for (int i = 0; i < p_abilityChargesMax; i++)
	{
		if(m_particleEmitters[i] != nullptr)
		{
			if (!m_particleEmitters[i]->emitterActiv)
			{
				delete m_particleEmitters[i];
				m_particleEmitters[i] = nullptr;
			}
		}
	}
}

void DisableAbility::_inStateCooldown(double dt)
{
	p_cooldown += dt;
	m_bar->setAngle(0);

	if (p_abilityCharges != 0)
	{
		if (p_cooldown >= 1)
		{
			m_dState = DisableState::Throwable;
			m_isActive = false;
		}
	}
	else
	{
		if (p_cooldown >= p_cooldownMax)
		{
			p_abilityCharges++;
			p_cooldown = 0;
			m_dState = DisableState::Throwable;
			m_isActive = false;
		}
	}
}

void DisableAbility::_inStateRemoteActive(double dt)
{
	static double accumulatedTime = 0;
	static const double lifeDuration = 1.0 / 0.2; //5000 ms
	accumulatedTime += dt;

	if (accumulatedTime >= (lifeDuration - delay))
	{
		//nothing has been hit within 5 seconds, -> reset
		accumulatedTime = 0.0;
		m_dState = DisableState::Throwable;
		m_obj->setPosition(-999.9f, -999.9f, -999.9f);
		return;
	}
}

void DisableAbility::_sendOnHitNotification(Enemy * ptr)
{
	Network::ENTITYSTATEPACKET packet(0,0,true);
	packet.id = Network::ID_ENEMY_DISABLED;
	packet.state = ptr->getUniqueID();
	packet.pos = ptr->getPosition();

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);
}

