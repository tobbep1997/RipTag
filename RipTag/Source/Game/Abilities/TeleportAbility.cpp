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
	PhysicsComponent::Init(*RipExtern::g_world, e_dynamicBody, 0.3f, 0.3f, 0.3f);
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

	m_bar = new Quad();
	Manager::g_textureManager.loadTextures("BAR");
	m_bar->init(DirectX::XMFLOAT2A(0.5f, 0.12f), DirectX::XMFLOAT2A(0.1f, 0.1f));
	Texture * texture = Manager::g_textureManager.getTexture("BAR");
	m_bar->setUnpressedTexture(texture);
	m_bar->setPivotPoint(Quad::PivotPoint::center);
	
	HUDComponent::AddQuad(m_bar);
	this->getBody()->SetObjectTag("TELEPORT");
	setManaCost(START_MANA_COST);
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
		if (((Player*)p_owner)->CheckManaCost(getManaCost()))
		{
			m_tpState = TeleportAbility::Charge;
		}
		break;
	case TeleportAbility::Teleport:
		DirectX::XMFLOAT4A position = Transform::getPosition();
		float velX = getLiniearVelocity().x;
		float velY = getLiniearVelocity().y;
		float velZ = getLiniearVelocity().z;

		DirectX::XMFLOAT4A direction = DirectX::XMFLOAT4A(getLiniearVelocity().x, getLiniearVelocity().y, getLiniearVelocity().z, 1);
		DirectX::XMFLOAT4A direction2 = DirectX::XMFLOAT4A(-getLiniearVelocity().x, -getLiniearVelocity().y, -getLiniearVelocity().z, 1);

		DirectX::XMFLOAT4A position1 = Transform::getPosition();
		DirectX::XMFLOAT4A position2 = Transform::getPosition();
		DirectX::XMFLOAT4A position3 = Transform::getPosition();
		DirectX::XMFLOAT4A position4 = Transform::getPosition();
		DirectX::XMFLOAT4A position5 = Transform::getPosition();
		DirectX::XMFLOAT4A position6 = Transform::getPosition();
		b3Vec3 bc;
		bc = RipExtern::m_rayListener->ShotRay2(getBody(), direction, 1);
		std::cout << bc.x << " " << bc.y << " " << bc.z << std::endl;

		position.y += 1.0f;

		position.x += bc.x;
		position.y += bc.y;
		position.z += bc.z;

		bc = RipExtern::m_rayListener->ShotRay2(getBody(), direction2, 1);
		std::cout << bc.x << " " << bc.y << " " << bc.z << std::endl;

		//position.y += 1.0f;

		position.x += bc.x;
		position.y += bc.y;
		position.z += bc.z;

		((Player*)p_owner)->setPosition(position.x, position.y, position.z, position.w);
		((Player*)p_owner)->setLiniearVelocity();
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
	HUDComponent::HUDDraw();
}

void TeleportAbility::_logic(double deltaTime)
{
	m_bar->setScale(0.f, 0.1f);
	if (m_useFunctionCalled) // the Use() function were called last frame
	{
		if (m_tpState == TeleportAbility::Charge)
		{
			m_bar->setScale(1.0f *(m_charge / MAX_CHARGE), .1f);
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

			((Player*)p_owner)->DrainMana(getManaCost());

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
