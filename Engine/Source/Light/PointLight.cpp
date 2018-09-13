#include "PointLight.h"
#include "../3D Engine/Extern.h"


PointLight::PointLight()
{
}


PointLight::~PointLight()
{
	for (int i = 0; i < sides.size(); i++)
	{
		delete sides[i];
	}
}

void PointLight::Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float power)
{
	this->position = position;
	this->color = color;
	this->dropOff = 1.0f - power;
	
	_createSides();
}

const DirectX::XMFLOAT4A & PointLight::getPosition()
{
	return position;
}

const DirectX::XMFLOAT4A & PointLight::getColor()
{
	return color;
}

const std::vector<Camera*>& PointLight::getSides()
{
	return sides;
}

const float & PointLight::getDropOff()
{
	return dropOff;
}

void PointLight::QueueLight()
{
	DX::g_lights.push_back(this);
}

void PointLight::SetPosition(const DirectX::XMFLOAT4A & pos)
{
	this->position = pos;
	_updateCameras();
}

void PointLight::SetPosition(float x, float y, float z, float w)
{
	this->SetPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::SetColor(const DirectX::XMFLOAT4A & color)
{
	this->color = color;
}

void PointLight::SetColor(float x, float y, float z, float w)
{
	this->SetColor(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::SetIntensity(float intencsity)
{
	this->dropOff = 1.0 - intencsity;
}

void PointLight::_createSides()
{
	using namespace DirectX;
	Camera * cam;

	float fov = 0.5f;
	float nearPlane = 1.0f, farPlane = 20.0f;

	cam = new Camera(XM_PI * fov, 1.0f, nearPlane, farPlane);
	cam->setPosition(this->position);
	cam->setUP(0, 0, 1);
	cam->setDirection(0, 1, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, nearPlane, farPlane);
	cam->setPosition(this->position);
	cam->setUP(1, 0, 0);
	cam->setDirection(0, -1, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, nearPlane, farPlane);
	cam->setPosition(this->position);
	cam->setDirection(1, 0, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, nearPlane, farPlane);
	cam->setPosition(this->position);
	cam->setDirection(-1, 0, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, nearPlane, farPlane);
	cam->setPosition(this->position);
	cam->setDirection(0, 0, 1);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, nearPlane, farPlane);
	cam->setPosition(this->position);
	cam->setDirection(0, 0, -1);
	sides.push_back(cam);

}

void PointLight::_updateCameras()
{
	for (unsigned int i = 0; i < sides.size(); i++)
	{
		sides[i]->setPosition(this->position);
	}
}
