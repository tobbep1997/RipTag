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
		break;
	case PointLight::Y_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		break;
	case PointLight::X_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		break;
	case PointLight::X_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		break;
	case PointLight::Z_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		break;
	case PointLight::Z_NEGATIVE:
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
	this->m_intensity = 1.0f;
	this->m_pow = 2.0f;
	//_createSides();
}

const DirectX::XMFLOAT4A & PointLight::getPosition() const
{
	return m_position;
}

const DirectX::XMFLOAT4A & PointLight::getColor() const
{
	return m_color;
}

const std::vector<Camera*>& PointLight::getSides() const
{
	return m_sides;
}

const float & PointLight::getDropOff() const
{
	return m_dropOff;
}

const float & PointLight::getPow() const
{
	return this->m_pow;
}

const float & PointLight::getIntensity() const
{
	return this->m_intensity;
}



void PointLight::CreateShadowDirection(const std::vector<ShadowDir> & shadowDir)
{
	for (unsigned int i = 0; i < shadowDir.size(); i++)
	{
		CreateShadowDirection(shadowDir[i]);
	}
}

float PointLight::TourchEffect(double deltaTime)
{
	static double time = 0.0f;
	static DirectX::XMFLOAT2 current(0.0, 0.0);
	static DirectX::XMFLOAT2 target(1.0, 1.0);
	static double timer = 0.0f;
	timer += deltaTime;
	static float ran = 5.5f;

	if (abs(current.x - target.x) < 0.1)
	{
		timer = 0.0;

		ran = (float)(rand() % 100) / 100.0f;

		target.x = ran;

	}

	auto v1 = DirectX::XMLoadFloat2(&current);
	auto v2 = DirectX::XMLoadFloat2(&target);
	DirectX::XMVECTOR vec;

	vec = DirectX::XMVectorLerp(v1, v2, deltaTime * 5);


	current.x = DirectX::XMVectorGetX(vec);

	float temp = 5 + sin(current.x) * 1.5;
	return temp;
}


void PointLight::QueueLight()
{
	DX::g_lights.push_back(this);
}

void PointLight::setPosition(const DirectX::XMFLOAT4A & pos)
{
	this->m_position = pos;
	_updateCameras();
}

void PointLight::setPosition(float x, float y, float z, float w)
{
	this->setPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::setColor(const DirectX::XMFLOAT4A & color)
{
	this->m_color = color;
}

void PointLight::setColor(float x, float y, float z, float w)
{
	this->setColor(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::setIntensity(float intencsity)
{
	this->m_intensity = intencsity;
}

void PointLight::setPower(float pow)
{
	this->m_pow = pow;
}

void PointLight::setDropOff(float dropOff)
{
	this->m_dropOff = dropOff;
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

float PointLight::getDistanceFromCamera(Camera& camera)
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
