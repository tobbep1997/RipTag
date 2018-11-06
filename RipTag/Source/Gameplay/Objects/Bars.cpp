#include "RipTagPCH.h"
#include "Bars.h"

Bars::Bars() : Triggerable()
{

}
Bars::Bars(int uniqueID, int linkedID, bool isTrigger) : Triggerable(uniqueID, linkedID, isTrigger, "activate", "activate")
{
}

void Bars::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float scaleX, float scaleY, float scaleZ)
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, scaleX, scaleY, scaleZ, false);
	BaseActor::setPositionRot(xPos, yPos - 3.f, zPos, pitch, yaw, roll);
	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("BARS");
	BaseActor::setModel(Manager::g_meshManager.getDynamicMesh("BARS"));
	auto& machine = getAnimatedModel()->InitStateMachine(1);
	getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("BARS"));
	machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("BARS", "BARS_ANIMATION").get());
	getAnimatedModel()->Pause();

	BaseActor::setUserDataBody(this);
}


void Bars::Update(double deltaTime)
{
	getAnimatedModel()->Update(deltaTime);
}

Bars::~Bars()
{
}
