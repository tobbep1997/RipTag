#include "DisableAbility.h"
#include "../../Actors/Player.h"


DisableAbility::DisableAbility(void* owner) : AbilityComponent(owner), BaseActor()
{
	m_dState = Throw;
	m_charge = 0.0f;
	m_travelSpeed = MAX_CHARGE;
	m_useFunctionCalled = false;
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

	setManaCost(START_MANA_COST);
}

void DisableAbility::Update(double deltaTime)
{
	BaseActor::Update(deltaTime);
	_logic(deltaTime);
}

void DisableAbility::Use()
{
	m_useFunctionCalled = true;
	switch (m_dState)
	{
	case DisableAbility::Throw:
		if (((Player*)p_owner)->CheckManaCost(getManaCost()))
		{
			m_dState = DisableAbility::Charge;
		}
		break;
	case DisableAbility::Moving:
		DirectX::XMFLOAT4A position = Transform::getPosition();
		position = { -999.0f, -999.0f, -999.0f, 1.0f };
		Transform::setPosition(position);
		m_dState = DisableAbility::Wait;
		break;
	}
}

void DisableAbility::Draw()
{
	if (m_dState == DisableAbility::Moving)
	{
		BaseActor::Draw();
	}
}

void DisableAbility::_logic(double deltaTime)
{
	if (m_useFunctionCalled) // the Use() function were called last frame
	{
		if (m_dState == DisableAbility::Charge)
		{
			if (m_charge < MAX_CHARGE)
				m_charge += deltaTime;
		}
	}
	else // the Use() function were not called last frame
	{
		switch (m_dState)
		{
		case DisableAbility::Wait:
			m_dState = DisableAbility::Throw;
			break;
		case DisableAbility::Charge:
			m_dState = DisableAbility::Moving;

			DirectX::XMFLOAT4A direction = ((Player *)p_owner)->getCamera()->getDirection();
			DirectX::XMFLOAT4A start = XMMATH::add(((Player*)p_owner)->getPosition(), direction);
			//This might be changed in the future

			((Player*)p_owner)->DrainMana(getManaCost());

			start.w = 1.0f;
			direction = XMMATH::scale(direction, TRAVEL_SPEED * m_charge);
			setPosition(start.x, start.y, start.z);
			setLiniearVelocity(direction.x, direction.y, direction.z);
			m_charge = 0.0f;
			break;
		case DisableAbility::Moving:
			bool cont = false;
			for (auto contact : RipExtern::m_contactListener->GetBeginContacts())
			{
				if (contact->GetShapeA()->GetBody()->GetObjectTag() == "Disable")
				{
					if (contact->GetShapeB()->GetBody()->GetObjectTag() == "Enemy")
					{
						Enemy * temp = static_cast<Enemy*>(contact->GetShapeB()->GetBody()->GetUserData());
						temp->DisableEnemy();
						//break;
					}
				}
				
			}
			
			break;
		}
	

	}

	m_useFunctionCalled = false;
}
