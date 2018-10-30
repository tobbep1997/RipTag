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
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 0.1f, 1.0f, true);
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
	for (int i = 0; i < RipExtern::m_contactListener->GetEndShapesA().size(); i++)
	{
		if (RipExtern::m_contactListener->GetEndShapesA()[i]->GetBody()->GetObjectTag() == "Player")
		{
			b = RipExtern::m_contactListener->GetEndShapesB()[i]->GetBody()->GetObjectTag() == "PressurePlate";
		}
		else if (RipExtern::m_contactListener->GetEndShapesA()[i]->GetBody()->GetObjectTag() == "PressurePlate")
		{
			b = RipExtern::m_contactListener->GetEndShapesB()[i]->GetBody()->GetObjectTag() == "Player";
		}
	}
	p_trigger(b);
}
