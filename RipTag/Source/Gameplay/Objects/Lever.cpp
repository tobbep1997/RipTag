#include "RipTagPCH.h"
#include "Lever.h"

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
	for (RayCastListener::RayContact* con : RipExtern::m_rayListener->GetContacts())
	{
		if (con->originBody->GetObjectTag() == "PLAYER" && con->contactShape->GetBody()->GetObjectTag() == getBody()->GetObjectTag())
		{
			if (static_cast<Lever*>(con->contactShape->GetBody()->GetUserData()) == this && *con->consumeState != 2)
			{
				p_trigger(!Triggerd());			
				*con->consumeState +=1;
			}
		}
	}
}
