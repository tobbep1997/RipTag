#include "RipTagPCH.h"
#include "Door.h"


Door::Door() : Triggerable()
{
}

Door::Door(int uniqueID, int linkedID, bool isTrigger) : Triggerable(uniqueID, linkedID, isTrigger, "activate", "activate")
{
}


Door::~Door()
{
}


void Door::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ)//TODO: ADD SCALE
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleX, bboxScaleY, bboxScaleZ, false);
	BaseActor::setPositionRot(xPos, yPos - 1, zPos, pitch, yaw, roll);
	//BaseActor::setPosition(xPos, yPos, zPos);

	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("Door");
	BaseActor::setModel(Manager::g_meshManager.getDynamicMesh("DOOR"));
	auto& machine = getAnimatedModel()->InitStateMachine(1);
	getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("DOOR"));
	machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("DOOR", "DOOR_ANIMATION").get());
	getAnimatedModel()->Pause();

	BaseActor::setUserDataBody(this);
}


void Door::Update(double deltaTime)
{
	getAnimatedModel()->Update(deltaTime);
	if (Triggerable::getState() == true)
		PhysicsComponent::p_setPosition(200, 200, 200);
}
