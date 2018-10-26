#include "TeleportAbility.h"
#include "../RipTagExtern/RipExtern.h"
#include "../Actors/Player.h"

TeleportAbility::TeleportAbility(void * owner) : AbilityComponent(owner), BaseActor()
{
	m_tpState = Throw;
	m_charge = 0.0f;
	m_travelSpeed = MAX_CHARGE;
	m_useFunctionCalled = false;
}

TeleportAbility::~TeleportAbility()
{
	PhysicsComponent::Release(*RipExtern::g_world);
}

void TeleportAbility::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_dynamicBody, 0.1f, 0.1f, 0.1f);
	Drawable::setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
	Drawable::setScale(0.1f, 0.1f, 0.1f);
	Drawable::setTexture(Manager::g_textureManager.getTexture("SPHERE"));
	BaseActor::setGravityScale(0.01f);
	Transform::setPosition(-999.0f, -999.0f, -999.0f);
	m_light.Init(Transform::getPosition(), COLOUR);

	if (USE_SHADOWS)
		m_light.CreateShadowDirection(PointLight::XYZ_ALL);
	
	m_light.setFarPlane(50.0f);
	m_light.setNearPlane(0.01f);
	m_light.setIntensity(10.0f);
	m_light.setDropOff(1.0f);

}

void TeleportAbility::Update(double deltaTime)
{
	BaseActor::Update(deltaTime);
	_updateLight();
	_logic(deltaTime);
}

void TeleportAbility::Use()
{
	m_useFunctionCalled = true;
	switch (m_tpState)
	{
	case TeleportAbility::Throw:
		m_tpState = TeleportAbility::Charge;
		break;
	case TeleportAbility::Teleport:
		DirectX::XMFLOAT4A position = Transform::getPosition();
		position.y += 1.0f;
		((Player*)p_owner)->setPosition(position.x, position.y, position.z, position.w);
		position = { -999.0f, -999.0f, -999.0f, 1.0f };
		m_tpState = TeleportAbility::Wait;
		break;
	}
}

void TeleportAbility::Draw()
{
	if (m_tpState == TeleportAbility::Teleport)
	{
		BaseActor::Draw();
		m_light.QueueLight();
	}
}

void TeleportAbility::_logic(double deltaTime)
{
	if (m_useFunctionCalled) // the Use() function were called last frame
	{
		if (m_tpState == TeleportAbility::Charge)
		{
			if (m_charge < MAX_CHARGE)
				m_charge += deltaTime;
		}
	}
	else // the Use() function were not called last frame
	{
		switch (m_tpState)
		{
		case TeleportAbility::Wait:
			m_tpState = TeleportAbility::Throw;
			break;
		case TeleportAbility::Charge:
			m_tpState = TeleportAbility::Teleport;
			
			DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
			DirectX::XMFLOAT4A start = XMMATH::add(((Player*)p_owner)->getPosition(), direction);
			start.w = 1.0f;
			direction = XMMATH::scale(direction, TRAVEL_SPEED * m_charge);
			setPosition(start.x, start.y, start.z);
			setLiniearVelocity(direction.x, direction.y, direction.z);
			m_charge = 0.0f;
			break;
		}
	}

	m_useFunctionCalled = false;
}

void TeleportAbility::_updateLight()
{
	m_light.setPosition(Transform::getPosition());
	//TODO :: LIGHT EFFECTS

}