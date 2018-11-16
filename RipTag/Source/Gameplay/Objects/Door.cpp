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
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleZ, bboxScaleY, bboxScaleX, true);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll); 
	//BaseActor::InitPositionRot(xPos, yPos - 1, zPos, pitch, yaw, roll);
	//BaseActor::setPosition(xPos, yPos, zPos);

	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("Door");
	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("DOOR"));
	BaseActor::setTexture(Manager::g_textureManager.getTexture("DOOR"));
	//auto& machine = getAnimatedModel()->InitStateMachine(1);
//	getAnimatedModel()->SetSkeleton(Manager::g_animationManager.getSkeleton("DOOR"));
	//machine->AddPlayOnceState("activate", Manager::g_animationManager.getAnimation("DOOR", "DOOR_ANIMATION").get());
	//getAnimatedModel()->Pause();

	BaseActor::setUserDataBody(this);
	//BaseActor::Update(0);
}


void Door::Update(double deltaTime)
{

	BaseActor::Update(deltaTime);

	if (Triggerable::getState() == true)
		PhysicsComponent::p_setPosition(200, 200, 200);
	
	//b3Mat33 mat = getBody()->GetTransform().rotation;
	//Transform::setPhysicsRotation(mat);//;->setPhysicsRotation(mat);
}
