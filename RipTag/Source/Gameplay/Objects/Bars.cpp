#include "RipTagPCH.h"
#include "Bars.h"

Bars::Bars() : Triggerable()
{

}
Bars::Bars(int uniqueID, int linkedID, bool isTrigger) : Triggerable(uniqueID, linkedID, isTrigger, "activate", "activate")
{
}

void Bars::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ)
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleX, bboxScaleY, bboxScaleZ, false);
	BaseActor::setPositionRot(xPos, yPos-5.f, zPos -3.f, pitch, yaw, roll);
	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("BARS");
	BaseActor::setModel(Manager::g_meshManager.getDynamicMesh("BARS"));
	auto& machine = getAnimationPlayer()->InitStateMachine(1);
	getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton("BARS"));
	machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("BARS", "BARS_ANIMATION").get());
	getAnimationPlayer()->Pause();

	BaseActor::setUserDataBody(this);
}


void Bars::Update(double deltaTime)
{
	getAnimationPlayer()->Update(deltaTime);
	if (Triggerable::getState() == true)
		PhysicsComponent::p_setPosition(200, 200, 200);
}

Bars::~Bars()
{
}
