#include "RipTagPCH.h"
#include "TeleportAbility.h"



TeleportAbility::TeleportAbility(void * owner) : AbilityComponent(owner), BaseActor(), HUDComponent()
{
	m_tpState = Throwable;
	m_charge = 0.0f;
	m_travelSpeed = MAX_CHARGE;
	m_boundingSphere = DBG_NEW DirectX::BoundingSphere(DirectX::XMFLOAT3(getPosition().x, getPosition().y, getPosition().z), .1f);
}

TeleportAbility::~TeleportAbility()
{
	delete m_light;
	delete m_boundingSphere;
	PhysicsComponent::Release(*RipExtern::g_world);
}

void TeleportAbility::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_dynamicBody, 0.3f, 0.3f, 0.3f);
	Drawable::setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	Drawable::setScale(0.1f, 0.1f, 0.1f);
	Drawable::setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	//OUTLINGING
	Drawable::setOutline(true);
	Drawable::setOutlineColor(DirectX::XMFLOAT4A(1, 0, 0, 1));

	BaseActor::setGravityScale(0.60f);
	Transform::setPosition(-999.0f, -999.0f, -999.0f);
	this->getBody()->SetObjectTag("TELEPORT");
	m_light = new PointLight();
	m_light->Init(Transform::getPosition(), COLOUR);

	if (USE_SHADOWS)
		m_light->CreateShadowDirection(PointLight::XYZ_ALL);
	
	m_light->setFarPlane(50.0f);
	m_light->setNearPlane(0.01f);
	m_light->setIntensity(10.0f);
	m_light->setDropOff(1.f); // set to 1

	m_bar = new Quad();
	Manager::g_textureManager.loadTextures("BAR");
	m_bar->init(DirectX::XMFLOAT2A(0.5f, 0.12f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	m_bar->setUnpressedTexture("BAR");
	m_bar->setPivotPoint(Quad::PivotPoint::center);
	
	HUDComponent::AddQuad(m_bar);
	
}

void TeleportAbility::Update(double deltaTime)
{
	if (m_tpState == TeleportState::Teleportable ||m_tpState == TeleportState::RemoteActive)
	{
		BaseActor::Update(deltaTime);
		_updateLight();
	}
	if (this->isLocal)
		_logicLocal(deltaTime);
	m_boundingSphere->Center = DirectX::XMFLOAT3(getPosition().x, getPosition().y, getPosition().z);

}

void TeleportAbility::UpdateFromNetwork(Network::ENTITYABILITYPACKET * data)
{
	switch (m_tpState)
	{
	case TeleportState::Throwable:
		if ((TeleportState)data->state == TeleportState::Teleportable)
		{
			DirectX::XMFLOAT4A pos = data->start;
			//adjust the start position based on the packets delay
			RakNet::Time delay = RakNet::GetTime() - data->timeStamp;
			delay *= 0.001; //delay is in ms but we are using seconds

			
			pos.x += data->velocity.x * delay;
			pos.y += data->velocity.y * delay;
			pos.z += data->velocity.z * delay;

			this->setPosition(pos.x, pos.y, pos.z);
			this->setLiniearVelocity(data->velocity.x, data->velocity.y, data->velocity.z);

			this->m_tpState = TeleportState::RemoteActive;
		}
		break;
	case TeleportState::RemoteActive:
		if ((TeleportState)data->state == TeleportState::Cooldown)
		{
			this->setPosition(-999.9f, -999.9f, -999.9f);
			this->setLiniearVelocity(0.0f, 0.0f, 0.0f);
			this->m_tpState = TeleportState::Throwable;
		}
		break;
	}
}

void TeleportAbility::Draw()
{
	switch (m_tpState)
	{
	case TeleportState::Charging:
		HUDComponent::HUDDraw();
		break;
	case TeleportState::Teleportable:
	case TeleportState::RemoteActive:
		BaseActor::Draw();
		m_light->QueueLight();
		break;
	}
}

unsigned int TeleportAbility::getState()
{
	return (unsigned int)m_tpState;
}

DirectX::XMFLOAT4A TeleportAbility::getVelocity()
{
	return this->m_lastVelocity;
}

DirectX::XMFLOAT4A TeleportAbility::getStart()
{
	return this->m_lastStart;
}

DirectX::BoundingSphere * TeleportAbility::GetBoundingSphere() const
{
	return m_boundingSphere;
}

void TeleportAbility::_logicLocal(double deltaTime)
{
	switch (m_tpState)
	{
	case TeleportState::Throwable:
		this->_inStateThrowable();
		break;
	case TeleportState::Charging:
		this->_inStateCharging(deltaTime);
		break;
	case TeleportState::Teleportable:
		this->_inStateTeleportable();
		break;
	case TeleportState::Cooldown:
		this->_inStateCooldown(deltaTime);
		break;
	}
}

void TeleportAbility::_inStateThrowable()
{
	if (isLocal)
	{
		if (((Player *)p_owner)->getCurrentAbility() == Ability::TELEPORT && Input::OnAbilityPressed())
		{
			
			m_tpState = TeleportAbility::Charging;
			
		}
	}
}

void TeleportAbility::_inStateCharging(double dt)
{
	if (isLocal)
	{
		if (((Player *)p_owner)->getCurrentAbility() == Ability::TELEPORT && Input::OnAbilityPressed())
		{
			m_bar->setScale(1.0f *(m_charge / MAX_CHARGE), .1f);
			if (m_charge < MAX_CHARGE)
				m_charge += dt;
		}
		if (((Player *)p_owner)->getCurrentAbility() != Ability::TELEPORT)
		{
			m_charge = 0.0;
			m_tpState = TeleportState::Throwable;
		}
		if (Input::OnAbilityReleased())
		{

			RayCastListener::Ray* ray = RipExtern::g_rayListener->ShotRay(getBody(), ((Player*)p_owner)->getCamera()->getPosition(), ((Player *)p_owner)->getCamera()->getDirection(), 1, true);
			if (ray == nullptr)
			{
				m_tpState = TeleportState::Teleportable;
				DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
				DirectX::XMFLOAT4A start = XMMATH::add(((Player*)p_owner)->getCamera()->getPosition(), direction);
				this->m_lastStart = start;

				

				start.w = 1.0f;
				direction = XMMATH::scale(direction, TRAVEL_SPEED * m_charge);
				setPosition(start.x, start.y, start.z);
				setLiniearVelocity(direction.x, direction.y, direction.z);
				this->m_lastVelocity = direction;
				m_charge = 0.0f;
			}
			else
			{
				m_tpState = TeleportState::Teleportable;
				DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
				DirectX::XMFLOAT4A start = XMMATH::subtract(((Player*)p_owner)->getCamera()->getPosition(), direction);
				this->m_lastStart = start;

				


				start.w = 1.0f;
				direction = XMMATH::scale(direction, TRAVEL_SPEED * m_charge);
				setPosition(start.x, start.y, start.z);
				setLiniearVelocity(direction.x, direction.y, direction.z);
				this->m_lastVelocity = direction;
				m_charge = 0.0f;
			}
		}
	}

}

void TeleportAbility::_inStateTeleportable()
{
	if (isLocal)
	{
		if (((Player *)p_owner)->getCurrentAbility() == Ability::TELEPORT && Input::OnAbilityPressed())
		{
			DirectX::XMFLOAT4A position = Transform::getPosition();

			DirectX::XMFLOAT4A dir;
			dir.x = getLiniearVelocity().x;
			dir.y = getLiniearVelocity().y;
			dir.z = getLiniearVelocity().z;
			dir.w = 1;

			DirectX::XMFLOAT4A dir2;
			dir2.x = -getLiniearVelocity().x;
			dir2.y = -getLiniearVelocity().y;
			dir2.z = -getLiniearVelocity().z;
			dir2.w = 1;

			RayCastListener::Ray* ray = RipExtern::g_rayListener->ShotRay(getBody(), getPosition(), dir, 2, true);
			RayCastListener::RayContact* var;
			if (ray != nullptr)
			{
				var = ray->getClosestContact();
				position.x += var->normal.x;
				position.y += var->normal.y;
				position.z += var->normal.z;
			}
			else
			{
				ray = RipExtern::g_rayListener->ShotRay(getBody(), getPosition(), dir2, 2, true);
				if (ray != nullptr)
				{
					var = ray->getClosestContact();
					position.x += var->normal.x;
					position.y += var->normal.y;
					position.z += var->normal.z;
				}
			}
			
			((Player*)p_owner)->setPosition(position.x, position.y, position.z, position.w);

			m_tpState = TeleportAbility::Cooldown;
		}
	}

}

void TeleportAbility::_inStateCooldown(double dt)
{
	//static double accumulatedTime = 0;
	//static const double cooldownDuration = 1.0 / 2.0; //500 ms
	p_cooldown += dt;
	if (p_cooldown >= p_cooldownMax)
	{
		p_cooldown = 0.0;
		m_tpState = TeleportState::Throwable;
	}
}

void TeleportAbility::_updateLight()
{
	m_light->setPosition(Transform::getPosition());
}
