#include "RipTagPCH.h"
#include "Bars.h"

Bars::Bars() : Triggerable()
{

}
Bars::Bars(int uniqueID, int linkedID, bool isTrigger) : Triggerable(uniqueID, linkedID, isTrigger)
{
}

void Bars::Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float scaleX, float scaleY, float scaleZ)
{
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, scaleX, scaleY, scaleZ, false);
	BaseActor::setPositionRot(xPos, yPos, zPos, pitch, yaw, roll);
	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("BARS");
	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("BARS"));
	BaseActor::setUserDataBody(this);
}


Bars::~Bars()
{
}
