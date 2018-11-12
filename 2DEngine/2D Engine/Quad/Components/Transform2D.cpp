#include "Engine2DPCH.h"
#include "Transform2D.h"



Transform2D::Transform2D()
{
}


Transform2D::~Transform2D()
{
}

void Transform2D::setPosition(const float & x, const float & y)
{
	this->setPosition(DirectX::XMFLOAT2A(x, y));
}

void Transform2D::setPosition(const DirectX::XMFLOAT2A & pos)
{
	this->m_position = pos;
}

const DirectX::XMFLOAT2A & Transform2D::getPosition() const
{
	return this->m_position;
}

void Transform2D::setScale(const float & width, const float & height)
{
	this->setScale(DirectX::XMFLOAT2A(width, height));
}

void Transform2D::setScale(const DirectX::XMFLOAT2A & size)
{
	this->m_size = size;
}

DirectX::XMFLOAT2 Transform2D::getScale() const
{
	return m_size;
}

const DirectX::XMFLOAT2A & Transform2D::getSize() const
{
	return this->m_size;
}

const DirectX::XMFLOAT4X4A & Transform2D::getWorldMatrix()
{
	using namespace DirectX;

	XMStoreFloat4x4A(&m_worldMatrix, XMMatrixTranspose(XMMatrixTranslation(m_position.x, m_position.y, 1)));
	return this->m_worldMatrix;
}
