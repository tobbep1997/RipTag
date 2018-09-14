#include "PointLight.h"
#include "../3D Engine/Extern.h"


PointLight::PointLight()
{
	m_nearPlane = 1.0f;
	m_farPlane = 20.0f;
}


PointLight::~PointLight()
{
	for (int i = 0; i < m_sides.size(); i++)
	{
		delete m_sides[i];
	}
}

void PointLight::CreateShadowDirection(ShadowDir direction)
{
	switch (direction)
	{
	case PointLight::Y_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f)); // UP
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Back
		break;
	case PointLight::Y_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Back
		break;
	case PointLight::X_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f)); // UP
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Back

		break;
	case PointLight::X_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f)); // UP
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Back
		break;
	case PointLight::Z_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f)); // UP
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		break;
	case PointLight::Z_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f)); // UP
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Back
		break;
	case PointLight::XYZ_ALL:
		this->_createSides();
		break;
	}


}

void PointLight::Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float power)
{
	this->m_position = position;
	this->m_color = color;
	this->m_dropOff = 1.0f - power;
	
	//_createSides();
}

const DirectX::XMFLOAT4A & PointLight::getPosition()
{
	return m_position;
}

const DirectX::XMFLOAT4A & PointLight::getColor()
{
	return m_color;
}

const std::vector<Camera*>& PointLight::getSides()
{
	return m_sides;
}

const float & PointLight::getDropOff()
{
	return m_dropOff;
}

void PointLight::QueueLight()
{
	DX::g_lights.push_back(this);
}

void PointLight::SetPosition(const DirectX::XMFLOAT4A & pos)
{
	this->m_position = pos;
	_updateCameras();
}

void PointLight::SetPosition(float x, float y, float z, float w)
{
	this->SetPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::SetColor(const DirectX::XMFLOAT4A & color)
{
	this->m_color = color;
}

void PointLight::SetColor(float x, float y, float z, float w)
{
	this->SetColor(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::SetIntensity(float intencsity)
{
	this->m_dropOff = 1.0f - intencsity;
}

void PointLight::setNearPlane(float nearPlane)
{
	for (unsigned int i = 0; i < m_sides.size(); i++)
	{
		this->m_sides[i]->setNearPlane(nearPlane);
	}
}

void PointLight::setFarPlane(float farPlane)
{
	for (unsigned int i = 0; i < m_sides.size(); i++)
	{
		this->m_sides[i]->setFarPlane(farPlane);
	}
}

float PointLight::GetDistanceFromCamera(Camera& camera)
{
	DirectX::XMVECTOR vec1 = DirectX::XMLoadFloat4A(&camera.getPosition());
	DirectX::XMVECTOR vec2 = DirectX::XMLoadFloat4A(&this->m_position);
	DirectX::XMVECTOR vecSubs = DirectX::XMVectorSubtract(vec1, vec2);
	DirectX::XMVECTOR lenght = DirectX::XMVector4Length(vecSubs);

	return DirectX::XMVectorGetX(lenght);

}

void PointLight::_createSides()
{
	using namespace DirectX;
	Camera * cam;

	float fov = 0.5f;
	

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setUP(0, 0, 1);
	cam->setDirection(0, 1, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setUP(1, 0, 0);
	cam->setDirection(0, -1, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(1, 0, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(-1, 0, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(0, 0, 1);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(0, 0, -1);
	m_sides.push_back(cam);

}

void PointLight::_createSide(const DirectX::XMFLOAT4A & dir, const DirectX::XMFLOAT4A & up)
{
	using namespace DirectX;
	Camera * cam;
	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setUP(up);
	cam->setDirection(dir);
	m_sides.push_back(cam);
}

void PointLight::_updateCameras()
{
	for (unsigned int i = 0; i < m_sides.size(); i++)
	{
		m_sides[i]->setPosition(this->m_position);
	}
}
