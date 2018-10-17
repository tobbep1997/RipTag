#include "CameraHolder.h"



void CameraHolder::p_initCamera(Camera * camera)
{
	this->p_camera = camera;
}

DirectX::XMFLOAT4A CameraHolder::p_CameraTilting(double deltaTime, float targetPeek, const DirectX::XMFLOAT4A & pos)
{
	using namespace DirectX;

	XMFLOAT4A forward = p_camera->getDirection();
	XMFLOAT4 UP = XMFLOAT4(0, 1, 0, 0);
	XMFLOAT4 RIGHT;
	//GeT_RiGhT;

	XMVECTOR vForward = XMLoadFloat4A(&forward);
	XMVECTOR vUP = XMLoadFloat4(&UP);
	XMVECTOR vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUP, vForward));
	vForward = XMVector3Normalize(XMVector3Cross(vRight, vUP));

	XMStoreFloat4A(&forward, vForward);
	XMStoreFloat4(&RIGHT, vRight);
	//float targetPeek = Input::PeekRight();

	XMVECTOR in = XMLoadFloat4A(&m_lastPeek);
	XMFLOAT4A none{ 0,1,0,0 };
	XMVECTOR target = XMLoadFloat4A(&none);

	XMMATRIX rot = DirectX::XMMatrixRotationAxis(vForward, (targetPeek * XM_PI / 8.0f));
	target = XMVector4Transform(target, rot);
	XMVECTOR out = XMVectorLerp(in, target, min(deltaTime * (m_peekSpeed + abs(targetPeek)), 1.0f));
	//out = XMVector4Transform(out, rot);
	XMStoreFloat4A(&m_lastPeek, out);
	p_camera->setUP(m_lastPeek);
	XMFLOAT4A cPos = p_camera->getPosition();
	XMVECTOR vToCam = XMVectorSubtract(DirectX::XMLoadFloat4A(&cPos), DirectX::XMLoadFloat4A(&pos));

	vToCam = XMVector4Transform(vToCam, rot);
	out = vToCam;
	//out = XMVectorLerp(vToCam, target, min(deltaTime * m_peekSpeed, 1.0f));
	out = XMVectorAdd(DirectX::XMLoadFloat4A(&pos), out);

	float MAX_PEEK = 1.0f;

	//XMVECTOR Step = XMVectorScale(vRight, min(deltaTime * (m_peekSpeed + abs(targetPeek)), MAX_PEEK));
	XMVECTOR sideStep = XMVectorLerp(DirectX::XMLoadFloat4A(&m_lastSideStep), XMVectorScale(vRight, -targetPeek * MAX_PEEK), min(deltaTime * (m_peekSpeed + abs(targetPeek)), 1.0f));
	XMStoreFloat4A(&m_lastSideStep, sideStep);
	out = XMVectorAdd(out, sideStep);
	XMStoreFloat4(&cPos, out);
	return cPos;
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
