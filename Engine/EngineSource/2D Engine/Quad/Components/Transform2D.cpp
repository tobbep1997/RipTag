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

const DirectX::XMFLOAT2A & Transform2D::getSize() const
{
	return this->m_size;
}
