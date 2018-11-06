#include "RipTagPCH.h"
#include "Door.h"


Door::Door() : Triggerable()
{
}

Door::Door(int uniqueID, int linkedID, bool isTrigger) : Triggerable(uniqueID, linkedID, isTrigger, "", "")
{
}


Door::~Door()
{
}


void Door::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float scaleX, float scaleY, float scaleZ)//TODO: ADD SCALE
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, scaleX, scaleY, scaleZ, false);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll);
	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("Door");
	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("DOOR"));
	
	
	BaseActor::setUserDataBody(this);
}

void Door::setPos(DirectX::XMFLOAT4A pos1, DirectX::XMFLOAT4A pos2)
{
	this->pos1 = pos1;
	
	this->pos2 = pos2;
}
