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
	m_openPos = { xPos, yPos + 8.0f, zPos , 1.0f };

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
	
	float t = deltaTime * 0.5f;

	if (m_wasClosed)
		m_timer += t;
	else
		m_timer -= t;

	m_timer = max(m_timer, 0.0f);
	m_timer = min(m_timer, 1.0f);

	DirectX::XMVECTOR v1, v2;
	v1 = DirectX::XMLoadFloat4A(&m_openPos);
	v2 = DirectX::XMLoadFloat4A(&m_closePos);
	if (Triggerable::getState() == true)
	{
		if (m_wasClosed)
		{
			m_wasClosed = false;
		}
		DirectX::XMVECTOR lerp = DirectX::XMVectorLerp(v1, v2, m_timer);
		DirectX::XMFLOAT3 openPos;
		DirectX::XMStoreFloat3(&openPos, lerp);
		setPosition(openPos.x, openPos.y, openPos.z);
		p_setPosition(openPos.x, openPos.y, openPos.z);
	}
	else
	{
		if (!m_wasClosed)
		{
			m_wasClosed = true;
		}
		DirectX::XMVECTOR lerp = DirectX::XMVectorLerp(v1, v2, m_timer);

		DirectX::XMFLOAT3 closePos;
		DirectX::XMStoreFloat3(&closePos, lerp);

		setPosition(closePos.x, closePos.y, closePos.z);
		p_setPosition(closePos.x, closePos.y, closePos.z);

	}
}

Bars::~Bars()
{
}
