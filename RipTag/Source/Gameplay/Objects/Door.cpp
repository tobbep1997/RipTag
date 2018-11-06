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


void Door::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float scaleX, float scaleY, float scaleZ)//TODO: ADD SCALE
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, 1, 2, 1, true);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll);
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
}
