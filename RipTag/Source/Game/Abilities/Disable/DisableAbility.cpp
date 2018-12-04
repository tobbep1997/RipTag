#include "RipTagPCH.h"
#include "DisableAbility.h"

DisableAbility::DisableAbility(void* owner) : AbilityComponent(owner), BaseActor()
{
	m_dState = Throwable;
	m_charge = 0.0f;
	m_travelSpeed = MAX_CHARGE;
}

DisableAbility::~DisableAbility()
{
	PhysicsComponent::Release(*RipExtern::g_world);
	//&this->deleteEffect(); 
	m_bar->Release();
	delete m_bar;
}

void DisableAbility::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_dynamicBody, 0.1f, 0.1f, 0.1f, false, 0.2f);
	Drawable::setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	Drawable::setScale(0.1f, 0.1f, 0.1f);
	Drawable::setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	BaseActor::setGravityScale(0.01f);
	Transform::setPosition(-999.0f, -999.0f, -999.0f);

	Drawable::setOutline(true);
	Drawable::setOutlineColor(DirectX::XMFLOAT4A(1, 0, 0, 1));

	PhysicsComponent::setUserDataBody(this);
	this->getBody()->SetObjectTag("Disable");

	m_bar = new Circle();
	Manager::g_textureManager.loadTextures("SPHERE");
	m_bar->init(DirectX::XMFLOAT2A(0.5f, 0.5f), DirectX::XMFLOAT2A(1.0f / 16.0f, 1.0f / 9.0f));
	m_bar->setUnpressedTexture("SPHERE");
	m_bar->setPivotPoint(Quad::PivotPoint::center);
	m_bar->setRadie(.5f);
	m_bar->setInnerRadie(.4);
	m_bar->setAngle(0);
}

void DisableAbility::deleteEffect()
{
	if (m_particleEmitter != nullptr)
	{
		delete m_particleEmitter;
		m_particleEmitter = nullptr;
	}
}

ParticleEmitter * DisableAbility::getEmitter()
{
	return m_particleEmitter; 
}

void DisableAbility::Update(double deltaTime)
{
	if (m_dState == DisableState::Moving || m_dState == DisableState::RemoteActive)
		BaseActor::Update(deltaTime);
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

			this->setPosition(pos.x, pos.y, pos.z);
			this->setLiniearVelocity(data->velocity.x, data->velocity.y, data->velocity.z);
			
			this->m_dState = DisableState::RemoteActive;
		}
		break;
	case DisableState::RemoteActive:
		if ((DisableState)data->state == DisableState::OnHit)
		{
			deleteEffect();

			this->setPosition(-999.9f, -999.9f, -999.9f);
			this->setLiniearVelocity(0.0f, 0.0f, 0.0f);
			this->m_dState = DisableState::Throwable;

			m_particleEmitter = new ParticleEmitter();
			m_particleEmitter->setSmoke();
			m_particleEmitter->setEmmiterLife(1.5f);
			m_particleEmitter->setPosition(data->start.x, data->start.y + 0.5f, data->start.z);
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
		BaseActor::Draw();
	}
	if (m_particleEmitter != nullptr)
		m_particleEmitter->Queue();
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

	if (m_particleEmitter != nullptr)
	{
		m_particleEmitter->Update(deltaTime, camera);
	}

}

void DisableAbility::_logicRemote(double dt, Camera * camera)
{
	if (m_dState == DisableAbility::RemoteActive)
		this->_inStateRemoteActive(dt);

	if (m_particleEmitter != nullptr)
	{
		m_particleEmitter->Update(dt, camera);
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
			((Player*)p_owner)->SetThrowing(true);
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
			((Player*)p_owner)->SetThrowing(false);
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("idle");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("bob");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("turn");
			m_charge = 0.0;
			p_cooldown = (p_cooldownMax / 3) * 2;
			m_dState = DisableState::Cooldown;
			m_canceled = true;
		}
		if (Input::OnAbility2Released())
		{
			m_dState = DisableState::Moving;

			if (Multiplayer::GetInstance()->isConnected())
			{
				Network::COMMONEVENTPACKET packet(Network::NETWORKMESSAGES::ID_PLAYER_THROW_END);
				Network::Multiplayer::SendPacket((const char*)&packet, sizeof(packet), PacketPriority::LOW_PRIORITY);
			}

			((Player*)p_owner)->SetThrowing(false);
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("throw_throw");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("bob");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("turn");
			DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
			DirectX::XMFLOAT4A start = XMMATH::add(((Player*)p_owner)->getCamera()->getPosition(), direction);
			this->m_lastStart = start;

			start.w = 1.0f;
			direction = XMMATH::scale(direction, TRAVEL_SPEED);
			setPosition(start.x, start.y, start.z);
			setLiniearVelocity(direction.x, direction.y, direction.z);
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
	static double accumulatedTime = 0;
	static const double lifeDuration = 1.0 / 0.2; //5000 ms

	//Set cooldown during wait
	accumulatedTime += dt;
	p_cooldown = accumulatedTime;

	ContactListener::S_Contact contact;
	for (int i = 0; i < (int)RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
	{
		if (!m_hasHit)
		{
			contact = RipExtern::g_contactListener->GetBeginContact(i);
			if (contact.a->GetBody()->GetObjectTag() == "Disable")
			{
				if (contact.b->GetBody()->GetObjectTag() == "ENEMY")
				{
					Enemy * ptr = static_cast<Enemy*>(contact.b->GetBody()->GetUserData());
					if (ptr)
					{
						if (ptr->getAIState() != AIState::Possessed && !ptr->ClientLocked())
						{
							m_hasHit = true; 
							m_isActive = true; 
					
							m_dState = DisableState::Cooldown;
							//Particle effects here before changing the position.  
							m_particleEmitter = new ParticleEmitter();
							m_particleEmitter->setSmoke(); 
							m_particleEmitter->setEmmiterLife(1.5f); 
							m_particleEmitter->setPosition(ptr->getPosition().x, ptr->getPosition().y + 0.5f, ptr->getPosition().z); 
							
							ptr->setTransitionState(AITransitionState::BeingDisabled);

							this->setPosition(-999.9f, -999.9f, -999.9f);
							p_cooldown = 0.0;
							accumulatedTime = 0.0;

							this->_sendOnHitNotification(ptr);

							FMOD_VECTOR at = FMOD_VECTOR{ this->getPosition().x, this->getPosition().y, this->getPosition().z };
							FMOD::Channel* c = nullptr;
							c = AudioEngine::PlaySoundEffect(RipSounds::g_smokeBomb, &at, &((Player*)p_owner)->getSmokeBombDesc());
						}
						else
						{
							m_hasHit = true;
							m_isActive = false;

							m_dState = DisableState::Cooldown;

							this->setPosition(-999.9f, -999.9f, -999.9f);
							p_cooldown = 0.0;
							accumulatedTime = 0.0;

							FMOD_VECTOR at = FMOD_VECTOR{ this->getPosition().x, this->getPosition().y, this->getPosition().z };
							FMOD::Channel* c = nullptr;
							c = AudioEngine::PlaySoundEffect(RipSounds::g_smokeBomb, &at, &((Player*)p_owner)->getSmokeBombDesc());
						}
					}
				}
			}
		}
	}

	if(m_particleEmitter != nullptr)
	{
		if (!m_particleEmitter->emitterActiv)
		{
			//Kill particle emitter
			delete m_particleEmitter;
			m_particleEmitter = nullptr;
		}
	}

	if (accumulatedTime >= lifeDuration)
	{
		//nothing has been hit within 5 seconds, -> reset
		accumulatedTime = 0.0;
		m_dState = DisableState::Cooldown;
		this->setPosition(-999.9f, -999.9f, -999.9f);
		return;
	}
}

void DisableAbility::_inStateCooldown(double dt)
{
	p_cooldown += dt;
	m_bar->setAngle(0);
	if (p_cooldown >= p_cooldownMax)
	{
		p_cooldown = 0;
		m_dState = DisableState::Throwable;
		m_hasHit = false;
		m_isActive = false;
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
		this->setPosition(-999.9f, -999.9f, -999.9f);
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

