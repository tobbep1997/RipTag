#include "EnginePCH.h"
#include "Transform.h"

float constexpr getNewValueInNewRange(float value, float rangeMin, float rangeMax, float newRangeMin, float newRangeMax)
{
	if (value > rangeMax)
	{
		return newRangeMax;
	}
	else
	{
		return ((value - rangeMin) * ((newRangeMax - newRangeMin) / (rangeMax - rangeMin))) + newRangeMin;
	}
}

float constexpr valueInRange(float value, float min, float max)
{
	return ((value - min) / (max - min));
}


void Transform::p_calcWorldMatrix()
{
	using namespace DirectX;
	XMMATRIX translation = DirectX::XMMatrixIdentity();
	XMMATRIX scaling = DirectX::XMMatrixIdentity();
	XMMATRIX rotation = DirectX::XMMatrixIdentity();
	translation = XMMatrixTranslation(this->p_position.x, this->p_position.y, this->p_position.z);
	scaling = XMMatrixScaling(this->p_scale.x, this->p_scale.y, this->p_scale.z);

	if (p_physicsRotation._11 == INT16_MIN)
	{
		rotation = XMMatrixRotationRollPitchYaw(this->p_rotation.x, this->p_rotation.y, this->p_rotation.z);
		//rotation = rotation * p_forcedRotation;
	}
	else
	{
		rotation = XMLoadFloat3x3(&p_physicsRotation);
	}
	XMMATRIX worldMatrix;
	if (DirectX::XMMatrixIsIdentity(p_forcedWorld))
		worldMatrix = XMMatrixTranspose(XMMatrixMultiply(m_modelTransform, scaling * rotation * translation));
	else
		worldMatrix = p_forcedWorld;
	//XMMATRIX worldMatrix = XMMatrixTranspose(rotation * scaling * translation);

	if (m_parent)
	{
		auto lol = m_parent->getWorldmatrix();
		worldMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&lol), worldMatrix);
	}

	
	XMStoreFloat4x4A(&this->p_worldMatrix, worldMatrix);

}

void Transform::p_calcWorldMatrixForOutline(const float & lenght)
{
	using namespace DirectX;

	float scaleBy = getNewValueInNewRange(lenght, 1.0f, 30.0f, 1.3f, 3.3f);
	//scaleBy = 1.3f;
	//valueInRange(lenght, 1.3f, 2.1f);
	
	XMMATRIX translation = XMMatrixTranslation(this->p_position.x, this->p_position.y, this->p_position.z);
	XMMATRIX scaling;
	
	scaling = XMMatrixScaling(this->p_scale.x * scaleBy, this->p_scale.y * scaleBy, this->p_scale.z * scaleBy);
	
	
	XMMATRIX rotation;
	rotation = DirectX::XMMatrixIdentity();
	if (p_physicsRotation._11 == INT16_MIN)
	{
		rotation = XMMatrixRotationRollPitchYaw(this->p_rotation.x, this->p_rotation.y, this->p_rotation.z);
		//rotation = rotation * p_forcedRotation;
	}
	else
	{
		rotation = XMLoadFloat3x3(&p_physicsRotation);
	}
	XMMATRIX worldMatrix;
	if (DirectX::XMMatrixIsIdentity(p_forcedWorld))
		worldMatrix = XMMatrixTranspose(XMMatrixMultiply(scaling * rotation * translation, m_modelTransform));
	else
		worldMatrix = p_forcedWorld;
	//XMMATRIX worldMatrix = XMMatrixTranspose(rotation * scaling * translation);

	if (m_parent)
	{
		auto lol = m_parent->getWorldmatrix();
		worldMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&lol), worldMatrix);
	}


	XMStoreFloat4x4A(&this->p_worldMatrix, worldMatrix);
}

void Transform::p_calcWorldMatrixForInsideOutline(const float& lenght)
{
	using namespace DirectX;

	float scaleBy = getNewValueInNewRange(lenght, 1.0f, 30.0f, 1.0f, 2.5f);
	//scaleBy = 1;
	XMMATRIX translation = XMMatrixTranslation(this->p_position.x, this->p_position.y, this->p_position.z);
	XMMATRIX scaling;

	scaling = XMMatrixScaling(this->p_scale.x * scaleBy, this->p_scale.y * scaleBy, this->p_scale.z * scaleBy);


	XMMATRIX rotation;
	rotation = DirectX::XMMatrixIdentity();
	if (p_physicsRotation._11 == INT16_MIN)
	{
		rotation = XMMatrixRotationRollPitchYaw(this->p_rotation.x, this->p_rotation.y, this->p_rotation.z);
		//rotation = rotation * p_forcedRotation;
	}
	else
	{
		rotation = XMLoadFloat3x3(&p_physicsRotation);
	}
	XMMATRIX worldMatrix;
	if (DirectX::XMMatrixIsIdentity(p_forcedWorld))
		worldMatrix = XMMatrixTranspose(XMMatrixMultiply(scaling * rotation * translation, m_modelTransform));
	else
		worldMatrix = p_forcedWorld;
	//XMMATRIX worldMatrix = XMMatrixTranspose(rotation * scaling * translation);

	if (m_parent)
	{
		auto lol = m_parent->getWorldmatrix();
		worldMatrix = XMMatrixMultiply(XMLoadFloat4x4A(&lol), worldMatrix);
	}


	XMStoreFloat4x4A(&this->p_worldMatrix, worldMatrix);
}

Transform::Transform()
{
	p_position = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_rotation = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_scale = DirectX::XMFLOAT4A(1, 1, 1, 1);
	p_forcedWorld = DirectX::XMMatrixIdentity();
	m_modelTransform = DirectX::XMMatrixIdentity();
	
	p_physicsRotation._11 = INT16_MIN;

	m_parent = nullptr;
}


Transform::~Transform()
{
}

void Transform::setParent(Transform & parent)
{
	this->m_parent = &parent;
}

const Transform & Transform::getParent() const
{
	return *this->m_parent;
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

void Transform::setModelTransform(const DirectX::XMMATRIX& matrix)
{
	m_modelTransform = matrix;
}

void Transform::ForceWorld(const DirectX::XMMATRIX & rot)
{
	p_forcedWorld = rot;
	p_calcWorldMatrix();
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

const Transform* Transform::getTransform()
{
	return this;
}


const DirectX::XMFLOAT4A & Transform::getPosition() const
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

DirectX::XMFLOAT4X4A Transform::getWorldmatrix()
{
	this->p_calcWorldMatrix();

	return this->p_worldMatrix;
}

DirectX::XMFLOAT4X4A Transform::getWorldMatrixForOutline(const DirectX::XMFLOAT4A & pos)
{
	float lenght = getLenghtToObject(pos);

	this->p_calcWorldMatrixForOutline(lenght);

	return this->p_worldMatrix;
}

DirectX::XMFLOAT4X4A Transform::getWorldMatrixForInsideOutline(const DirectX::XMFLOAT4A& pos)
{
	float lenght = getLenghtToObject(pos);

	this->p_calcWorldMatrixForInsideOutline(lenght);

	return this->p_worldMatrix;
}

void Transform::setPhysicsRotation(const b3Mat33 & rot)
{
	p_physicsRotation._11 = rot.x.x;
	p_physicsRotation._12 = rot.x.y;
	p_physicsRotation._13 = rot.x.z;

	p_physicsRotation._21 = rot.y.x;
	p_physicsRotation._22 = rot.y.y;
	p_physicsRotation._23 = rot.y.z;

	p_physicsRotation._31 = rot.z.x;
	p_physicsRotation._32 = rot.z.y;
	p_physicsRotation._33 = rot.z.z;
}

float Transform::getLenghtToObject(const DirectX::XMFLOAT4A& pos)
{
	DirectX::XMVECTOR vec1 = DirectX::XMLoadFloat4A(&pos);
	DirectX::XMVECTOR vec2 = DirectX::XMLoadFloat4A(&this->p_position);
	DirectX::XMVECTOR vecSubs = DirectX::XMVectorSubtract(vec1, vec2);
	DirectX::XMVECTOR lenght = DirectX::XMVector4Length(vecSubs);

	return DirectX::XMVectorGetX(lenght);
}

// #todoREMOVE
void Transform::setVelocity(b3Vec3 vel)
{
	m_Velocity = vel;
}
