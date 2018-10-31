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

void PressurePlate::Update(double deltaTime)
{
	p_updatePhysics(this);

	for (ContactListener::S_EndContact con : RipExtern::m_contactListener->GetEndContacts())
	{
		if (con.a->GetBody()->GetObjectTag() == "Player" &&
			con.b->GetBody()->GetObjectTag() == "PressurePlate")
		{
			p_trigger(false);
		}
	}
	for (b3Contact * con : RipExtern::m_contactListener->GetBeginContacts())
	{
		if (con->GetShapeA()->GetBody()->GetObjectTag() == "Player" &&
			con->GetShapeB()->GetBody()->GetObjectTag() == "PressurePlate")
		{
			p_trigger(true);
		}
	}	
}
