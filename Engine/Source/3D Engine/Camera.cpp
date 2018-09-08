#include "Camera.h"

void Camera::_calcViewMatrix()
{
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat4A(&this->m_position);
	DirectX::XMVECTOR direction = DirectX::XMLoadFloat4A(&this->m_direction);
	DirectX::XMVECTOR up = DirectX::XMLoadFloat4A(&this->m_UP);

	DirectX::XMStoreFloat4x4A(&this->m_view, DirectX::XMMatrixTranspose(DirectX::XMMatrixLookToLH(pos, direction, up)));
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
}

void Camera::setDirection(float x, float y, float z, float w)
{
	this->setDirection(DirectX::XMFLOAT4A(x, y, z, w));
}

DirectX::XMFLOAT4X4A Camera::getView()
{
	_calcViewMatrix();
	return this->m_view;
}

DirectX::XMFLOAT4X4A Camera::getProjection()
{
	return this->m_projection;
}

DirectX::XMFLOAT4X4A Camera::getViewProjection()
{
	_calcViewProjectionMatrix();
	return this->m_viewProjection;
}
