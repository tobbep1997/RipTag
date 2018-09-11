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

void Camera::setPosition(DirectX::XMFLOAT4A pos)
{
	this->m_position = pos;
}

void Camera::setPosition(float x, float y, float z, float w)
{
	this->setPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void Camera::setDirection(DirectX::XMFLOAT4A direction)
{
	this->m_direction = direction;
	this->m_usingDir = true;
}

void Camera::setDirection(float x, float y, float z, float w)
{
	this->setDirection(DirectX::XMFLOAT4A(x, y, z, w));
}

void Camera::setLookTo(DirectX::XMFLOAT4A pos)
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
