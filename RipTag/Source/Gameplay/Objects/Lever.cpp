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
				if (Triggerd())
				{
					p_trigger(false);
				}
				else
				{
					p_trigger(true);
				}
			}
		}
	}
}