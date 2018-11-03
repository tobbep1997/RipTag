#include "EnginePCH.h"
#include "CameraHolder.h"


void CameraHolder::p_initCamera(Camera * camera)
{
	this->p_camera = camera;
}

double CameraHolder::p_viewBobbing(double deltaTime, double velocity, double moveSpeed, MoveState moveState)
{
	if (this->p_moveState == Walking)
	{
		m_moveBob += (float)(velocity * (moveSpeed * deltaTime)); 
		m_offset = m_moveAmp * sin(m_moveFreq * m_moveBob); 
	}
	else if (this->p_moveState == Sprinting)
	{
		m_sprintBob += (float)(velocity * (moveSpeed * deltaTime));
		m_offset = m_sprintAmp * sin(m_sprintFreq * m_sprintBob);
	}
	else
	{
		m_stopBob += (float)deltaTime * 1.5f; 
		m_offset = m_stopAmp * sin(m_stopFreq * m_stopBob);
	}

	return m_offset;
}

DirectX::XMFLOAT4A CameraHolder::p_CameraTilting(double deltaTime, float targetPeek, const DirectX::XMFLOAT4A & pos)
{
	using namespace DirectX;

	XMFLOAT4A forward = p_camera->getDirection();
	XMFLOAT4 UP = XMFLOAT4(0, 1, 0, 0);
	XMFLOAT4 RIGHT;

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
	XMVECTOR out = XMVectorLerp(in, target, min((float)deltaTime * (m_peekSpeed + std::abs(targetPeek)), 1.0f));

	XMStoreFloat4A(&m_lastPeek, out);
	p_camera->setUP(m_lastPeek);
	XMFLOAT4A cPos = p_camera->getPosition();
	XMVECTOR vToCam = XMVectorSubtract(DirectX::XMLoadFloat4A(&cPos), DirectX::XMLoadFloat4A(&pos));

	vToCam = XMVector4Transform(vToCam, rot);
	out = vToCam;

	out = XMVectorAdd(DirectX::XMLoadFloat4A(&pos), out);

	float MAX_PEEK = 1.0f;


	XMVECTOR sideStep = XMVectorLerp(DirectX::XMLoadFloat4A(&m_lastSideStep), XMVectorScale(vRight, -targetPeek * MAX_PEEK), min((float)deltaTime * (m_peekSpeed + abs(targetPeek)), 1.0f));
	XMStoreFloat4A(&m_lastSideStep, sideStep);
	out = XMVectorAdd(out, sideStep);
	XMStoreFloat4(&cPos, out);
	return cPos;
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

CameraHolder::CameraHolder()
{
}


CameraHolder::~CameraHolder()
{
	delete this->p_camera;
}

Camera * CameraHolder::getCamera() const
{
	return this->p_camera;
}
