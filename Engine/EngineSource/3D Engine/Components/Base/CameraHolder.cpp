#include "EnginePCH.h"
#include "CameraHolder.h"



void CameraHolder::p_initCamera(Camera * camera)
{
	this->p_camera = camera;
}

double CameraHolder::p_viewBobbing(double deltaTime, double moveSpeed, b3Body * owner)
{
	using namespace DirectX;

	XMFLOAT4A cPos = p_camera->getPosition();
	
	//Remove all m_offsetY so we get the real point
	//std::cout << cPos.x << ", " << cPos.y << ", " << cPos.z << std::endl;
	XMFLOAT4A cDir = p_camera->getDirection();
	XMFLOAT4A cRight = p_camera->getRight();
	
	cPos.y += m_offsetY;

	XMFLOAT2 xmHorizontal = { cRight.x * m_offsetX, cRight.z * m_offsetX };

	if (moveSpeed > 0.1f)
	{
		cPos.x += xmHorizontal.x;
		cPos.z += xmHorizontal.y;
	}

	XMVECTOR lookAt = XMVectorAdd(XMLoadFloat4A(&cPos), XMVectorScale(XMLoadFloat4A(&cDir), 5.0f));
	XMVECTOR vPoint = lookAt;
	if(m_rayId == -100)
		m_rayId = RipExtern::g_rayListener->PrepareRay(owner, cPos, cDir, 100);
	else
	{
		if (RipExtern::g_rayListener->hasRayHit(m_rayId))
		{
			RayCastListener::Ray * ray = RipExtern::g_rayListener->GetProcessedRay(m_rayId);
			XMFLOAT4A point;
			b3Vec3 vec = ray->getClosestContact()->contactPoint;
			//std::cout << ray->getClosestContact()->originBody->GetObjectTag() << std::endl;
			point = { vec.x, vec.y, vec.z, 1.0f };
			vPoint = XMLoadFloat4A(&point);
		}
		m_rayId = -100;
	}

	cPos.y -= m_offsetY;
	if (moveSpeed > 0.1f)
	{
		cPos.x -= xmHorizontal.x;
		cPos.z -= xmHorizontal.y;
	}

	if (m_horAdder)
		m_horizontalBob += (moveSpeed * SPEED_CONSTANT + IDLE_CONSTANT) * deltaTime;
	else
		m_horizontalBob -= (moveSpeed * SPEED_CONSTANT + IDLE_CONSTANT) * deltaTime;
	
	if (m_verAdder)
		m_verticalBob += (moveSpeed * SPEED_CONSTANT + IDLE_CONSTANT) * deltaTime;
	else
		m_verticalBob -= (moveSpeed * SPEED_CONSTANT + IDLE_CONSTANT) * deltaTime;

	if (m_verticalBob > XM_PI + (moveSpeed <= 0.01f) * XM_PIDIV2)
		m_verAdder = false;
	else if (m_verticalBob < 0)
		m_verAdder = true;

	if (m_horizontalBob > XM_PI * 1.5f)
		m_horAdder = false;
	else if (m_horizontalBob < DirectX::XM_PIDIV2)
		m_horAdder = true;




	float verCurve = sin(m_verticalBob); 
	float horCurve = sin(m_horizontalBob);

	if (moveSpeed > 0.01f)
		m_offsetY = verCurve * VERTICAL_AMPLIFIER;
	else
	{
		m_offsetY = verCurve * VERTICAL_AMPLIFIER * 0.1f;
	}

	
	if (moveSpeed > 0.01f)
	{
		m_offsetX = horCurve * HORIZONTAL_AMPLIFIER;
		xmHorizontal = { cRight.x * m_offsetX, cRight.z * m_offsetX };
		cPos.x += xmHorizontal.x;
		cPos.z += xmHorizontal.y;
	}
	cPos.y += m_offsetY;

	XMVECTOR vPos = XMLoadFloat4(&cPos);
	XMVECTOR newDir = XMVectorSubtract(vPoint, vPos);
	if (XMVectorGetX(XMVector3Length(newDir)) < 5.0f)
	{
		newDir = XMVectorSubtract(lookAt, XMLoadFloat4(&cPos));
	}

	XMFLOAT4A xmDir;
	XMStoreFloat4A(&xmDir, XMVector3Normalize(newDir));
	xmDir.w = 0.0f;
	p_camera->setDirection(xmDir);
	p_camera->setPosition(cPos);
	return m_offsetY;
}

DirectX::XMFLOAT4A CameraHolder::p_CameraTilting(double deltaTime, float targetPeek)
{
	using namespace DirectX;

	XMFLOAT4A forward = p_camera->getDirection();
	XMFLOAT4 UP = XMFLOAT4(0, 1, 0, 0);
	XMFLOAT4A RIGHT;

	XMVECTOR vForward = XMLoadFloat4A(&forward);
	XMVECTOR vUP = XMLoadFloat4(&UP);
	XMVECTOR vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUP, vForward));
	vForward = XMVector3Normalize(XMVector3Cross(vRight, vUP));

	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&RIGHT, vRight);

	XMVECTOR in = XMLoadFloat4A(&m_lastPeek);
	XMFLOAT4A none{ 0,1,0,0 };
	XMVECTOR target = XMLoadFloat4A(&none);

	XMMATRIX rot = DirectX::XMMatrixRotationAxis(vForward, (targetPeek * XM_PI / 8.0f));
	target = XMVector4Transform(target, rot);
	//XMVECTOR out = XMVectorLerp(in, target, min((float)deltaTime * (m_peekSpeed*2 + std::abs(targetPeek)), 1.0f));

	XMStoreFloat4A(&m_lastPeek, target);
	p_camera->setUP(m_lastPeek);

	//XMFLOAT4A cPos = p_camera->getPosition(); 
	/*XMVECTOR vToCam = XMVectorSubtract(DirectX::XMLoadFloat4A(&cPos), DirectX::XMLoadFloat4A(&pos));

	vToCam = XMVector4Transform(vToCam, rot);
	out = vToCam;

	out = XMVectorAdd(DirectX::XMLoadFloat4A(&pos), out);

	float MAX_PEEK = 1.0f;


	XMVECTOR sideStep = XMVectorLerp(DirectX::XMLoadFloat4A(&m_lastSideStep), XMVectorScale(vRight, -targetPeek * MAX_PEEK), min((float)deltaTime * (m_peekSpeed + abs(targetPeek)), 1.0f));
	XMStoreFloat4A(&m_lastSideStep, sideStep);
	out = XMVectorAdd(out, sideStep);
	XMStoreFloat4(&cPos, out);*/
	return RIGHT;
}

double CameraHolder::p_Crouching(double deltaTime, float& startHeight, const DirectX::XMFLOAT4A & pos)
{
	if (startHeight != 0)
	{
		if (m_lastHeight != 0 && startHeight != m_lastHeight) //Animation did not finish and inverts step direction
		{
			m_crouchAnimSteps = abs(1 - m_crouchAnimSteps);
		}
		DirectX::XMFLOAT4A startPos = pos;
		startPos.y = startPos.y + (startHeight - startPos.y);
		m_crouchAnimSteps += (float)deltaTime * m_crouchSpeed;
		if (m_crouchAnimSteps < 1) //Animation ongoing
		{
			DirectX::XMStoreFloat4A(&startPos, DirectX::XMVectorLerp(DirectX::XMLoadFloat4A(&startPos), DirectX::XMLoadFloat4A(&pos), m_crouchAnimSteps));
			m_lastHeight = startHeight;
		}
		else //Animation Finished
		{
			m_crouchAnimSteps = 0;
			m_lastHeight = 0;
			startHeight = 0;
			DirectX::XMStoreFloat4A(&startPos, DirectX::XMVectorLerp(DirectX::XMLoadFloat4A(&startPos), DirectX::XMLoadFloat4A(&pos), 1));
		}

		return startPos.y - pos.y;
	}
	return 0;
}

DirectX::XMFLOAT4A CameraHolder::getForward() const
{
	
	return p_camera->getForward();

}

CameraHolder::CameraHolder()
{
}


CameraHolder::~CameraHolder()
{
	delete this->p_camera;
}

DirectX::XMFLOAT4A CameraHolder::getLastPeek() const
{
	return m_lastPeek;
}

void CameraHolder::setLastPeek(DirectX::XMFLOAT4A peek)
{
	m_lastPeek = peek;
}

Camera * CameraHolder::getCamera() const
{
	return this->p_camera;
}
