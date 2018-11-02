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
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1.0f, 1.0f, 1.0f, true);
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
		if ((con.a->GetBody()->GetObjectTag() == "PLAYER" || con.a->GetBody()->GetObjectTag() == "ENEMY") &&
			con.b->GetBody()->GetObjectTag() == "PressurePlate")
		{
			p_trigger(false);
		}
	}
	for (b3Contact * con : RipExtern::m_contactListener->GetBeginContacts())
	{
		if ((con->GetShapeA()->GetBody()->GetObjectTag() == "PLAYER" || con->GetShapeA()->GetBody()->GetObjectTag() == "ENEMY") &&
			con->GetShapeB()->GetBody()->GetObjectTag() == "PressurePlate")
		{

			p_trigger(true);
		}
	}	

	if (Triggerd())
		BaseActor::setPosition(pos2.x, pos2.y, pos2.z);
	else
		BaseActor::setPosition(pos1.x, pos1.y, pos1.z);

}

void PressurePlate::setPos(DirectX::XMFLOAT4A trigg, DirectX::XMFLOAT4A unTrigg)
{
	pos1 = trigg;
	pos2 = unTrigg;
}
