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
	PhysicsComponent::Init(*RipExtern::g_world, e_staticBody, bboxScaleZ / 2, bboxScaleY / 2, bboxScaleX / 2, false);

	BaseActor::setPosition(xPos, yPos, zPos);
	BaseActor::setRotation(pitch, yaw, roll, false);
	BaseActor::setPhysicsRotation(pitch, 90, roll);
	m_startRotModel = { pitch, yaw, roll };
	m_startRotBox = { pitch, 90 , roll };
	if (yaw >= 0.0f)
	{
		m_closePos = { xPos + bboxScaleX / 2, yPos + bboxScaleY / 2, zPos , 1.0f };
		m_openPos = { xPos, yPos + bboxScaleY / 2, zPos - bboxScaleX / 2 , 1.0f };
	}
	else
	{
		m_closePos = { xPos, yPos + bboxScaleY / 2, zPos + bboxScaleZ / 2, 1.0f };
		m_openPos = { xPos + bboxScaleZ / 2, yPos + bboxScaleY / 2, zPos, 1.0f };
	}


	BaseActor::setScale(scaleX, scaleY, scaleZ);
	BaseActor::setObjectTag("DOOR");
	BaseActor::setModel(Manager::g_meshManager.getStaticMesh("DOOR"));
	BaseActor::setTexture(Manager::g_textureManager.getTexture("DOOR"));
	BaseActor::setUserDataBody(this);

}


void Door::Update(double deltaTime)
{
	BaseActor::Update(deltaTime);
	if (Triggerable::getState() == true)
	{
		setRotation(m_startRotModel.x, m_startRotModel.y + 90, m_startRotModel.z, false);
		setPhysicsRotation(m_startRotBox.x, m_startRotBox.y + 90, m_startRotBox.z);
		p_setPosition(m_openPos.x, m_openPos.y, m_openPos.z);
	}
	else
	{
		setRotation(m_startRotModel.x, m_startRotModel.y, m_startRotModel.z, false);
		setPhysicsRotation(m_startRotBox.x, m_startRotBox.y, m_startRotBox.z);
		p_setPosition(m_closePos.x, m_closePos.y, m_closePos.z);
	}
}
