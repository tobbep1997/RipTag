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
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleZ, bboxScaleY, bboxScaleX, false);
	//BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll); 
	//BaseActor::InitPositionRot(xPos, yPos - 1, zPos, pitch, yaw, roll);
	//BaseActor::setPosition(xPos, yPos, zPos);

	BaseActor::setPosition(xPos, yPos, zPos);
	BaseActor::setRotation(pitch, yaw, roll, false);
	BaseActor::setPhysicsRotation(pitch, yaw + 90, roll);

	m_startRotModel = { pitch, yaw, roll };
	m_startRotBox = { pitch, yaw + 90, roll };


	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("DOOR");
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
	{
		//setRotation(m_startRotModel.x, m_startRotModel.y + 90, m_startRotModel.z, false);
		setPhysicsRotation(m_startRotBox.x, m_startRotBox.y + 90, m_startRotBox.z);
	}
	else
	{
		//setRotation(m_startRotModel.x, m_startRotModel.y, m_startRotModel.z, false);
		setPhysicsRotation(m_startRotBox.x, m_startRotBox.y, m_startRotBox.z);
	}
	
	//b3Mat33 mat = getBody()->GetTransform().rotation;
	//Transform::setPhysicsRotation(mat);//;->setPhysicsRotation(mat);
}
