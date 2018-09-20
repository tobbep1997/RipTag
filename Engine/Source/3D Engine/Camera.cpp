#include "Camera.h"

void Camera::_calcViewMatrix(bool dir)
{
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat4A(&this->m_position);
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
	this->m_position = DirectX::XMFLOAT4A(0, 0, 0, 1);
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

void Camera::setPosition(const DirectX::XMFLOAT4A & pos)
{
	this->m_position = pos;
}

void Camera::setPosition(float x, float y, float z, float w)
{
	DirectX::XMFLOAT4A pos(x, y, z, w);
	this->setPosition(pos);
}

void Camera::Translate(const DirectX::XMFLOAT4A & pos)
{

	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4A(&this->m_direction);

	if (!this->m_usingDir)
		vDir = DirectX::XMVectorSubtract(vDir, DirectX::XMLoadFloat4A(&this->m_position));


	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat4A(&this->m_UP);
	DirectX::XMVECTOR vRight = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vUp, vDir));
	
	DirectX::XMFLOAT3 right;
	DirectX::XMStoreFloat3(&right, vRight);

	m_position.x += pos.x * right.x;
	m_position.y += pos.x * right.y;
	m_position.z += pos.x * right.z;

	m_position.x += pos.y * m_UP.x;
	m_position.y += pos.y * m_UP.y;
	m_position.z += pos.y * m_UP.z;

	m_position.x += pos.z * m_direction.x;
	m_position.y += pos.z * m_direction.y;
	m_position.z += pos.z * m_direction.z;


	this->m_position.w = 1.0f;
}

void Camera::Translate(float x, float y, float z, float w)
{
	this->Translate(DirectX::XMFLOAT4A( x, y, z, w ));
}

void Camera::Rotate(const DirectX::XMFLOAT4A & rotation)
{
	DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4A(&this->m_direction);
	DirectX::XMVECTOR vLastDir = vDir;
	if (!this->m_usingDir)
	{
		vDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(vDir, DirectX::XMLoadFloat4A(&this->m_position)));
		vLastDir = vDir;
		this->m_usingDir = true;
	}

	DirectX::XMVECTOR vUp = DirectX::XMLoadFloat4A(&this->m_UP);
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
	if (dot > -0.99f && dot < 0.99f)
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

const DirectX::XMFLOAT4A & Camera::getPosition() const
{
	return m_position;
}

const DirectX::XMFLOAT4A & Camera::getDirection() const
{
	return this->m_direction;
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
