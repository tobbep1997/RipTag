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
	this->getBody()->SetObjectTag("Enemy");

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
		m_dState = DisableAbility::Charge;
		break;
	case DisableAbility::Moving:
		DirectX::XMFLOAT4A position = Transform::getPosition();
		position = { -999.0f, -999.0f, -999.0f, 1.0f };
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
				/*if (contact->GetShapeA()->GetBody()->GetUserData() == this)
				{
					std::cout << "self" << std::endl;
					
				}
*/
				if (contact->GetShapeA()->GetBody()->GetObjectTag() == "Dis")
				{
					if (contact->GetShapeB()->GetBody()->GetObjectTag() == "Eny")
					{
						Enemy * temp = static_cast<Enemy*>(contact->GetShapeB()->GetBody()->GetUserData());
						temp->DisableEnemy();
					}
				}
				//std::cout << contact->GetShapeA()->GetBody()->GetUserData() << std::endl;
				//std::cout << contact->GetShapeB()->GetBody()->GetUserData() << std::endl;
				//if (static_cast<DisableAbility*>(contact->GetShapeA()->GetBody()->GetUserData()) != nullptr)
				//{
				//	//dynamic_cast<DisableAbility*>(contact->GetShapeA()->GetBody()->GetUserData());
				//	void* v = contact->GetShapeB()->GetBody()->GetUserData();
				//	if (v == nullptr || (long long)v == 0xFFFFFFFFCCCCCCCC)
				//		std::cout << "FAK" << std::endl;
				//	Enemy * temp = static_cast<Enemy*>(contact->GetShapeB()->GetBody()->GetUserData());
				//	Enemy * oo = temp->validate();
				//	if (oo && (long long)v != 0xFFFFFFFFCCCCCCCC)
				//	{
				//		
				//		//RipExtern::lol = oo;
				//		oo->DisableEnemy();
				//		//std::cout << contact->GetShapeA()->GetBody()->GetUserData() << std::endl;
				//		//std::cout << contact->GetShapeB()->GetBody()->GetUserData() << std::endl;
				//		break;
				//	}
				//	
				//}
				/*if (static_cast<Enemy*>(contact->GetShapeA()->GetBody()->GetUserData()))
				{
					if (static_cast<DisableAbility*>(contact->GetShapeB()->GetBody()->GetUserData()))
					{
						
						cont = true;
						break;
					}
				}*/
				
			}
			/*if (cont == true)
			{
				
			}*/
			break;
		}
	

	}

	m_useFunctionCalled = false;
}
