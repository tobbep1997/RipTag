#include "Lever.h"
#include "../../../RipTagExtern/RipExtern.h"
#include <iostream>

Lever::Lever()
{
}


Lever::~Lever()
{
}

void Lever::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 1.0f, 1.0f, true);
	p_setPosition(getPosition().x, getPosition().y, getPosition().z);
	setObjectTag("LEVER");
	setUserDataBody(this);
}

void Lever::BeginPlay()
{

}

void Lever::Update(double deltaTime)
{
	p_updatePhysics(this);
	for (RayCastListener::RayContact con : RipExtern::m_rayListener->GetContacts())
	{
		if (con.originBody->GetObjectTag() == "PLAYER")
		{
			if (con.contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
			{
				if (static_cast<Lever*>(con.contactShape->GetBody()->GetUserData()) == this)
				{
					p_trigger(!Triggerd());			
					*con.consumeState +=1;
				}
			}
		}
	}
	//std::cout << Triggerd() << std::endl;
}

bool Lever::isEqual(Lever * target)
{
	if (this->getPosition().x == target->getPosition().x && 
		this->getPosition().y == target->getPosition().y && 
		this->getPosition().z == target->getPosition().z)
		return true;

	return false;
}
