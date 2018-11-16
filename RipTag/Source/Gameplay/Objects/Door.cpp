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
	m_timer += deltaTime;
	m_timer = min(m_timer, 1.0f);

	DirectX::XMVECTOR vp1, vp2, vrm1, vrm2, vrb1, vrb2;
	
	DirectX::XMFLOAT3 xmrMe(m_startRotModel), xmrBe(m_startRotBox);
	xmrMe.y += 90;
	xmrBe.y += 90;

	vp1 = DirectX::XMLoadFloat4A(&m_closePos);
	vp2 = DirectX::XMLoadFloat4A(&m_openPos);

	vrm1 = DirectX::XMLoadFloat3(&m_startRotModel);
	vrm2 = DirectX::XMLoadFloat3(&xmrMe);

	vrb1 = DirectX::XMLoadFloat3(&m_startRotBox);
	vrb2 = DirectX::XMLoadFloat3(&xmrBe);

	if (Triggerable::getState() == true)
	{
		if (m_wasClosed)
		{
			m_wasClosed = false;
			m_timer = 0.0f;
		}
		DirectX::XMFLOAT3 xmCurrentModelRotation, xmCurrentBoundingRotation, xmCurrentBoundingPos;

		DirectX::XMVECTOR vCurrentModelRotation, vCurrentBoundingRotation, vCurrentBoundingPos;
		vCurrentModelRotation = DirectX::XMVectorLerp(vrm1, vrm2, m_timer);
		vCurrentBoundingRotation = DirectX::XMVectorLerp(vrb1, vrb2, m_timer);
		vCurrentBoundingPos = DirectX::XMVectorLerp(vp1, vp2, m_timer);

		DirectX::XMStoreFloat3(&xmCurrentModelRotation, vCurrentModelRotation);
		DirectX::XMStoreFloat3(&xmCurrentBoundingRotation, vCurrentBoundingRotation);
		DirectX::XMStoreFloat3(&xmCurrentBoundingPos, vCurrentBoundingPos);

		setRotation(xmCurrentModelRotation.x, xmCurrentModelRotation.y, xmCurrentModelRotation.z, false);
		setPhysicsRotation(xmCurrentBoundingRotation.x, xmCurrentBoundingRotation.y, xmCurrentBoundingRotation.z);
		p_setPosition(xmCurrentBoundingPos.x, xmCurrentBoundingPos.y, xmCurrentBoundingPos.z);
	}
	else
	{
		if (!m_wasClosed)
		{
			m_wasClosed = true;
			m_timer = 0.0f;
		}

		DirectX::XMFLOAT3 xmCurrentModelRotation, xmCurrentBoundingRotation, xmCurrentBoundingPos;

		DirectX::XMVECTOR vCurrentModelRotation, vCurrentBoundingRotation, vCurrentBoundingPos;
		vCurrentModelRotation = DirectX::XMVectorLerp(vrm2, vrm1, m_timer);
		vCurrentBoundingRotation = DirectX::XMVectorLerp(vrb2, vrb1, m_timer);
		vCurrentBoundingPos = DirectX::XMVectorLerp(vp2, vp1, m_timer);

		DirectX::XMStoreFloat3(&xmCurrentModelRotation, vCurrentModelRotation);
		DirectX::XMStoreFloat3(&xmCurrentBoundingRotation, vCurrentBoundingRotation);
		DirectX::XMStoreFloat3(&xmCurrentBoundingPos, vCurrentBoundingPos);

		setRotation(xmCurrentModelRotation.x, xmCurrentModelRotation.y, xmCurrentModelRotation.z, false);
		setPhysicsRotation(xmCurrentBoundingRotation.x, xmCurrentBoundingRotation.y, xmCurrentBoundingRotation.z);
		p_setPosition(xmCurrentBoundingPos.x, xmCurrentBoundingPos.y, xmCurrentBoundingPos.z);
	}
}
