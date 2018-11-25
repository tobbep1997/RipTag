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
}

void DisableAbility::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_dynamicBody, 0.1f, 0.1f, 0.1f);
	Drawable::setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	Drawable::setScale(0.1f, 0.1f, 0.1f);
	Drawable::setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	BaseActor::setGravityScale(0.01f);
	Transform::setPosition(-999.0f, -999.0f, -999.0f);

	PhysicsComponent::setUserDataBody(this);
	this->getBody()->SetObjectTag("Disable");

	m_bar = new Quad();
	Manager::g_textureManager.loadTextures("BAR");
	m_bar->init(DirectX::XMFLOAT2A(0.5f, 0.12f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	Texture * texture = Manager::g_textureManager.getTexture("BAR");
	m_bar->setUnpressedTexture("BAR");
	m_bar->setPivotPoint(Quad::PivotPoint::center);

	HUDComponent::AddQuad(m_bar);
}

void DisableAbility::deleteEffect()
{
	if (m_particleEmitter != nullptr)
	{
		delete m_particleEmitter;
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
		_logicRemote(deltaTime);
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
			this->setPosition(-999.9f, -999.9f, -999.9f);
			this->setLiniearVelocity(0.0f, 0.0f, 0.0f);
			this->m_dState = DisableState::Throwable;
		}
		break;
	}
}

void DisableAbility::Use()
{
}

void DisableAbility::Draw()
{
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

void DisableAbility::_logicRemote(double dt)
{
	if (m_dState == DisableAbility::RemoteActive)
		this->_inStateRemoteActive(dt);
}

void DisableAbility::_inStateThrowable()
{
	if (isLocal)
	{
		if (!m_canceled && ((Player *)p_owner)->getCurrentAbility() == Ability::DISABLE && Input::OnAbility2Pressed())
		{
			
			m_dState = DisableAbility::Charging;
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("throw_ready");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->PopLayer("bob");
		}
		if (Input::OnAbility2Released())
		{
			m_canceled = false;
		}
	}
}

void DisableAbility::_inStateCharging(double dt)
{
	if (isLocal)
	{
		if (((Player *)p_owner)->getCurrentAbility() == Ability::DISABLE && Input::OnAbility2Pressed())
		{
			m_bar->setScale(1.0f *(m_charge / MAX_CHARGE), .1f);
			if (m_charge < MAX_CHARGE)
				m_charge += dt;
		}
		if (Input::OnCancelAbilityPressed())
		{
			m_charge = 0.0;
			m_dState = DisableState::Throwable;
			m_canceled = true;
		}
		if (Input::OnAbility2Released())
		{
			m_dState = DisableState::Moving;
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetStateMachine()->SetState("throw_throw");
			((Player*)p_owner)->GetFirstPersonAnimationPlayer()->GetLayerMachine()->ActivateLayer("bob");
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
	accumulatedTime += dt;
	p_cooldown = accumulatedTime;
	ContactListener::S_Contact contact;
	for (int i = 0; i < RipExtern::g_contactListener->GetNrOfBeginContacts(); i++)
	{
		if (!m_hasHit)
		{
			contact = RipExtern::g_contactListener->GetBeginContact(i);
			if (contact.a->GetBody()->GetObjectTag() == "Disable")
			{
				if (contact.b->GetBody()->GetObjectTag() == "ENEMY")
				{
					m_hasHit = true; 
					static_cast<Enemy*>(contact.b->GetBody()->GetUserData())->setTransitionState(AITransitionState::BeingDisabled);
					m_dState = DisableState::Cooldown;
					//Particle effects here before changing the position.  
					m_particleEmitter = new ParticleEmitter();
					m_particleEmitter->setPosition(this->getPosition().x, this->getPosition().y, this->getPosition().z);
					std::cout << "X: " << DirectX::XMVectorGetX(m_particleEmitter->getPosition()) << " Y: " << DirectX::XMVectorGetY(m_particleEmitter->getPosition()) << " Z: " << DirectX::XMVectorGetZ(m_particleEmitter->getPosition()) << std::endl;
					this->setPosition(-999.9f, -999.9f, -999.9f);
					p_cooldown = 0.0;
					accumulatedTime = 0.0;
					this->_sendOnHitNotification();
				}
			}
		}
	}

	if (accumulatedTime >= lifeDuration)
	{
		//nothing has been hit within 5 seconds, -> reset
		accumulatedTime = 0.0;
		p_cooldown = 0.0;
		m_dState = DisableState::Throwable;
		this->setPosition(-999.9f, -999.9f, -999.9f);
		return;
	}
}

void DisableAbility::_inStateCooldown(double dt)
{
	p_cooldown += dt;
	if (p_cooldown >= p_cooldownMax)
	{
		p_cooldown = 0;
		m_dState = DisableState::Throwable;
		delete m_particleEmitter; 
		m_particleEmitter = nullptr; 
		m_hasHit = false;
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

void DisableAbility::_sendOnHitNotification()
{
	Network::ENTITYABILITYPACKET packet;
	packet.id = Network::ID_PLAYER_ABILITY;
	packet.ability = (unsigned int)Ability::DISABLE;
	packet.state = (unsigned int)DisableState::OnHit;

	Network::Multiplayer::SendPacket((const char*)&packet, sizeof(Network::ENTITYABILITYPACKET), PacketPriority::LOW_PRIORITY);
}

