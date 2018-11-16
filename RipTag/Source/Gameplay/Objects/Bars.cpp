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
	BaseActor::setPosition(xPos, yPos, zPos);
	BaseActor::setRotation(pitch, yaw, roll, false);
	BaseActor::setPhysicsRotation(pitch, 0, roll);
	
		m_closePos = { xPos, yPos, zPos , 1.0f };
		m_openPos = { xPos, yPos + bboxScaleY * 2, zPos , 1.0f };





	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("BARS");
	BaseActor::setTexture(Manager::g_textureManager.getTexture("BARS"));

	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("BARS"));
	BaseActor::setUserDataBody(this);
	m_closePos = { xPos, yPos, zPos, 1 };
}


void Bars::Update(double deltaTime)
{
	BaseActor::Update(deltaTime);
	if (Triggerable::getState() == true)
	{
		p_setPosition(m_openPos.x, m_openPos.y, m_openPos.z);
	}
	else
	{
		p_setPosition(m_closePos.x, m_closePos.y, m_closePos.z);
	}
}

Bars::~Bars()
{
}
