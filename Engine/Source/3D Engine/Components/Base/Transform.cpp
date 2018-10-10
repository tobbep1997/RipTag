#include "Transform.h"



void Transform::p_calcWorldMatrix()
{
	using namespace DirectX;
	XMMATRIX translation = XMMatrixTranslation(this->p_position.x, this->p_position.y, this->p_position.z);
	XMMATRIX scaling = XMMatrixScaling(this->p_scale.x, this->p_scale.y, this->p_scale.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw(this->p_rotation.x, this->p_rotation.y, this->p_rotation.z);
	
	DirectX::XMStoreFloat4x4A(&this->p_worldMatrix, XMMatrixTranspose(rotation * scaling * translation));

}

Transform::Transform()
{
	p_position = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_rotation = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_scale = DirectX::XMFLOAT4A(1, 1, 1, 1);
}


Transform::~Transform()
{
}


void Transform::setPosition(const DirectX::XMFLOAT4A & pos)
{
	this->p_position = pos;
}

void Transform::setPosition(const float & x, const float & y, const float & z, const float & w)
{
	this->setPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void Transform::Translate(const DirectX::XMFLOAT3 & translation)
{
	this->p_position.x += translation.x;
	this->p_position.y += translation.y;
	this->p_position.z += translation.z;
}

void Transform::Translate(const float & x, const float & y, const float & z)
{
	this->p_position.x += x;
	this->p_position.y += y;
	this->p_position.z += z;
}


void Transform::setScale(const DirectX::XMFLOAT4A & scale)
{
	this->p_scale = scale;
}

void Transform::setScale(const float & x, const float & y, const float & z, const float & w)
{
	this->setScale(DirectX::XMFLOAT4A(x, y, z, w));
}

void Transform::addScale(const DirectX::XMFLOAT3 & scale)
{
	this->p_scale.x += scale.x;
	this->p_scale.y += scale.y;
	this->p_scale.z += scale.z;
}

void Transform::addScale(const float & x, const float & y, const float & z)
{
	this->p_scale.x += x;
	this->p_scale.y += y;
	this->p_scale.z += z;
}


void Transform::setRotation(const DirectX::XMFLOAT4A & rot)
{
	this->p_rotation = rot;
}

void Transform::setRotation(const float & x, const float & y, const float & z, const float & w)
{
	this->setRotation(DirectX::XMFLOAT4A(x, y, z, w));
}

void Transform::addRotation(const DirectX::XMFLOAT4A & rot)
{
	this->p_rotation.x += rot.x;
	this->p_rotation.y += rot.y;
	this->p_rotation.z += rot.z;
	this->p_rotation.w += rot.w;
}

void Transform::addRotation(const float & x, const float & y, const float & z, const float & w)
{
	this->p_rotation.x += x;
	this->p_rotation.y += y;
	this->p_rotation.z += z;
	this->p_rotation.w += w;
}


const DirectX::XMFLOAT4A  & Transform::getPosition() const
{
	return this->p_position;
}

const DirectX::XMFLOAT4A & Transform::getScale() const
{
	return this->p_scale;
}

const DirectX::XMFLOAT4A & Transform::getEulerRotation() const
{
	return this->p_rotation;
}
