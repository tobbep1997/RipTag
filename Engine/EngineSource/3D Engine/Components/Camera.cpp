#include "Camera.h"
#include <iostream>
#include <math.h>
#include <algorithm>

const DirectX::XMFLOAT4A Camera::getYRotationEuler()
{
	using namespace DirectX;

	XMVECTOR XZcameraDirection = XMVector3Normalize(XMVectorSet(m_direction.x, 0.0, m_direction.z, 0.0));
	XMVECTOR defaultDir = XMVectorSet(0.0, 0.0, -1.0, 0.0);
	float dot = XMVectorGetX(XMVector3Dot(defaultDir, XZcameraDirection));

	dot = std::clamp(dot, -0.999999f, 0.999999f);
	//Convert to degrees
	float r = std::acos(dot);
	//Negate if necessary
	float inverter = (XMVectorGetY(XMVector3Cross(XZcameraDirection, defaultDir)));

	r *= (inverter > 0.0)
		? -1.0
		: 1.0;
	r = std::clamp(r, -XM_PI, XM_PI);

	return {0.0, r, 0.0, 0.0};
}

void Camera::_calcViewMatrix(bool dir)
{
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat4A(&this->p_position);
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat4A(&this->m_direction);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4A(&this->m_UP);

	if (dir)
		DirectX::XMStoreFloat4x4A(&this->m_view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookToLH(pos, direction, up)));
	else
		DirectX::XMStoreFloat4x4A(&this->m_view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(pos, direction, up)));
}

void Camera::_calcProjectionMatrix()
{
	DirectX::XMStoreFloat4x4A(&this->m_projection, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearPlane, m_farPlane)));
}

void Camera::_calcViewProjectionMatrix()
{
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4A(&this->m_view);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4A(&this->m_projection);

	DirectX::XMStoreFloat4x4A(&this->m_viewProjection, proj * view);
}

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	this->p_position = DirectX::XMFLOAT4A(0, 0, 0, 1);
	this->m_direction = DirectX::XMFLOAT4A(0, 0, 1, 0);
	this->m_UP = DirectX::XMFLOAT4A(0, 1, 0, 0);
	this->m_fov = fov;
	this->m_aspectRatio = aspectRatio;
	this->m_nearPlane = nearPlane;
	this->m_farPlane = farPlane;
	this->m_usingDir = true;

	_calcViewMatrix();
	_calcProjectionMatrix();
}

Camera::~Camera()
{
}


void Camera::Translate(const DirectX::XMFLOAT4A & pos)
{

	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4A(&this->m_direction);

	if (!this->m_usingDir)
		vDir = DirectX::XMVectorSubtract(vDir, DirectX::XMLoadFloat4A(&this->p_position));


	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat4A(&this->m_UP);
	DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vDir));
	
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, vRight);

	p_position.x += pos.x * right.x;
	p_position.y += pos.x * right.y;
	p_position.z += pos.x * right.z;	

	p_position.x += pos.y * m_UP.x;
	p_position.y += pos.y * m_UP.y;
	p_position.z += pos.y * m_UP.z;

	p_position.x += pos.z * m_direction.x;
	p_position.y += pos.z * m_direction.y;
	p_position.z += pos.z * m_direction.z;


	this->p_position.w = 1.0f;
}

void Camera::Translate(float x, float y, float z, float w)
{
	this->Translate(DirectX::XMFLOAT4A(x, y, z, w));
}


void Camera::Rotate(const DirectX::XMFLOAT4A & rotation)
{
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4A(&this->m_direction);
	DirectX::XMVECTOR vLastDir = vDir;
	if (!this->m_usingDir)
	{
		vDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vDir, DirectX::XMLoadFloat4A(&this->p_position)));
		vLastDir = vDir;
		this->m_usingDir = true;
	}

	DirectX::XMFLOAT4A l_UP(0, 1, 0, 0);

	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat4A(&l_UP);
	DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vDir));

	vRight = DirectX::XMVectorScale(vRight, rotation.x);
	vUp = DirectX::XMVectorScale(vUp, rotation.y);
	vDir = DirectX::XMVectorScale(vDir, rotation.z);

	DirectX::XMVECTOR vRot = DirectX::XMVectorAdd(vRight, vUp);
	vRot = DirectX::XMVectorAdd(vRot, vDir);

	DirectX::XMMATRIX mRot = DirectX::XMMatrixRotationRollPitchYawFromVector(vRot);
	
	vDir = DirectX::XMLoadFloat4A(&this->m_direction);

	DirectX::XMVECTOR vNewDir = DirectX::XMVector3TransformNormal(vLastDir, mRot);
	vUp = DirectX::XMLoadFloat4A(&this->m_UP);


	DirectX::XMVECTOR vDot = DirectX::XMVector3Dot(vNewDir, vUp);
	float dot = DirectX::XMVectorGetX(vDot);
	DirectX::XMStoreFloat4A(&this->m_direction, DirectX::XMVector3Normalize(vNewDir));
}

void Camera::Rotate(float x, float y, float z, float w)
{
	this->Rotate(DirectX::XMFLOAT4A(x, y, z, w));
}

void Camera::setDirection(const DirectX::XMFLOAT4A & direction)
{
	this->m_direction = direction;
	this->m_usingDir = true;
}

void Camera::setDirection(float x, float y, float z, float w)
{
	this->setDirection(DirectX::XMFLOAT4A(x, y, z, w));
}

void Camera::setLookTo(const DirectX::XMFLOAT4A & pos)
{
	this->m_direction = pos;
	this->m_usingDir = false;
}

void Camera::setLookTo(float x, float y, float z, float w)
{
	this->setLookTo(DirectX::XMFLOAT4A(x, y, z, w));
}

void Camera::setUP(DirectX::XMFLOAT4A up)
{
	this->m_UP = up;
}

void Camera::setUP(float x, float y, float z, float w)
{
	this->setUP(DirectX::XMFLOAT4A(x, y, z, w));
}

void Camera::setNearPlane(float nearPlane)
{
	this->m_nearPlane = nearPlane;
	_calcProjectionMatrix();
}

void Camera::setFarPlane(float farPlane)
{
	this->m_farPlane = farPlane;
	_calcProjectionMatrix();
}

const float & Camera::getFarPlane() const
{
	return this->m_farPlane;
}
const float & Camera::getNearPlane() const
{
	return this->m_nearPlane;
}
const float & Camera::getFOV() const
{
	return this->m_fov;
}

const DirectX::XMFLOAT4A & Camera::getDirection() const
{
	return this->m_direction;
}

const DirectX::XMFLOAT4A & Camera::getRight() const
{
	DirectX::XMVECTOR dir, up, right;
	dir = DirectX::XMLoadFloat4A(&this->m_direction);
	up = DirectX::XMLoadFloat4A(&this->m_UP);
	right = DirectX::g_XMZero;

	DirectX::XMFLOAT4A toReturn;
	right = DirectX::XMVector3Cross(up, dir);
	right = DirectX::XMVector3Normalize(right);

	DirectX::XMStoreFloat4A(&toReturn, right);

	return toReturn;
}

const DirectX::XMFLOAT4X4A & Camera::getView()
{
	_calcViewMatrix(m_usingDir);
	return this->m_view;
}

const DirectX::XMFLOAT4X4A & Camera::getProjection() const
{
	return this->m_projection;
}

const DirectX::XMFLOAT4X4A & Camera::getViewProjection()
{
	_calcViewMatrix(m_usingDir);
	_calcViewProjectionMatrix();
	return this->m_viewProjection;
}

DirectX::XMFLOAT4A Camera::_add(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b)
{
	DirectX::XMVECTOR vA, vB;
	vA = DirectX::XMLoadFloat4A(&a);
	vB = DirectX::XMLoadFloat4A(&b);
	DirectX::XMVECTOR vSum = DirectX::XMVectorAdd(vA, vB);
	DirectX::XMFLOAT4A sum;
	DirectX::XMStoreFloat4A(&sum, vSum);
	return sum;
}
