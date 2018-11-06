#include "RipTagPCH.h"
#include "Triggerable.h"

Triggerable::Triggerable()
{
}


Triggerable::~Triggerable()
{
}

void Triggerable::BeginPlay()
{
}

void Triggerable::Update(double deltaTime)
{
}

void Triggerable::Release()
{
	PhysicsComponent::Release(*RipExtern::g_world);
}

Triggerable::Triggerable(int uniqueId, int linkedID, bool isTrigger)
{

}
