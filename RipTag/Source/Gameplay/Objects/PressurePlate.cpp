#include "PressurePlate.h"
#include "../../../RipTagExtern/RipExtern.h"


PressurePlate::PressurePlate() : Trigger(), BaseActor()
{
	
}


PressurePlate::~PressurePlate()
{
}

void PressurePlate::Init()
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 0.1f, 1.0f, false);
	setObjectTag("PressurePlate");
	setUserDataBody(this);
}

void PressurePlate::BeginPlay()
{
	
}
#include <iostream>
void PressurePlate::Update(double deltaTime)
{
	p_updatePhysics(this);
	bool b = false;
	for (auto contact : RipExtern::m_contactListener->GetPersistingContacts())
	{
		if (contact->GetShapeA()->GetBody()->GetObjectTag() == "Player")
		{
			if (contact->GetShapeB()->GetBody()->GetObjectTag() == "PressurePlate")
			{
				b = true;
			}
		}
	}
	p_trigger(b);
	

}
