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
	m_bar->setUnpressedTexture(texture);
	m_bar->setPivotPoint(Quad::PivotPoint::center);

	HUDComponent::AddQuad(m_bar);
	setManaCost(START_MANA_COST);
}

void DisableAbility::Update(double deltaTime)
{
	if (m_dState == DisableState::Moving || m_dState == DisableState::RemoteActive)
		BaseActor::Update(deltaTime);

	if (this->isLocal)
		_logicLocal(deltaTime);
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

void DisableAbility::_logicLocal(double deltaTime)
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
		if (Input::OnAbilityPressed())
		{
			if (((Player*)p_owner)->CheckManaCost(getManaCost()))
			{
				m_dState = DisableAbility::Charging;
			}
		}
	}
}

void DisableAbility::_inStateCharging(double dt)
{
	if (isLocal)
	{
		if (Input::OnAbilityPressed())
		{
			m_bar->setScale(1.0f *(m_charge / MAX_CHARGE), .1f);
			if (m_charge < MAX_CHARGE)
				m_charge += dt;
		}
		if (Input::OnAbilityReleased())
		{
			m_dState = DisableState::Moving;
			DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
			DirectX::XMFLOAT4A start = XMMATH::add(((Player*)p_owner)->getCamera()->getPosition(), direction);
			this->m_lastStart = start;

			((Player*)p_owner)->DrainMana(getManaCost());

			start.w = 1.0f;
			direction = XMMATH::scale(direction, TRAVEL_SPEED * m_charge);
			setPosition(start.x, start.y, start.z);
			setLiniearVelocity(direction.x, direction.y, direction.z);
			m_lastVelocity = direction;
			m_charge = 0.0f;
		}
	}
}

void DisableAbility::_inStateMoving(double dt)
{
	static double accumulatedTime = 0;
	static const double lifeDuration = 1.0 / 0.2; //5000 ms
	accumulatedTime += dt;

	for (auto contact : RipExtern::m_contactListener->GetBeginContacts())
	{
		if (contact->GetShapeA()->GetBody()->GetObjectTag() == "Disable")
		{
			if (contact->GetShapeB()->GetBody()->GetObjectTag() == "ENEMY")
			{
				Enemy * temp = static_cast<Enemy*>(contact->GetShapeB()->GetBody()->GetUserData());
				temp->DisableEnemy();
				m_dState = DisableState::Throwable;
				this->setPosition(-999.9f, -999.9f, -999.9f);
				this->_sendOnHitNotification();
				
			}
		}
	}

	if (accumulatedTime >= lifeDuration)
	{
		//nothing has been hit within 5 seconds, -> reset
		accumulatedTime = 0.0;
		m_dState = DisableAbility::Throwable;
		this->setPosition(-999.9f, -999.9f, -999.9f);
		return;
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
		m_dState = DisableAbility::Throwable;
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

