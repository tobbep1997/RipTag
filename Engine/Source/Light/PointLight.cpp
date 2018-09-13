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

void PointLight::Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float dropOff)
{
	this->position = position;
	this->color = color;
	this->dropOff = dropOff;
	
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

void PointLight::_createSides()
{
	using namespace DirectX;
	Camera * cam;

	float fov = 0.5f;


	cam = new Camera(XM_PI * fov, 1.0f, 1.0f, 10.0f);
	cam->setPosition(this->position);
	cam->setUP(0, 0, 1);
	cam->setDirection(0, 1, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, 1.0f, 10.0f);
	cam->setPosition(this->position);
	cam->setUP(1, 0, 0);
	cam->setDirection(0, -1, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, 1.0f, 10.0f);
	cam->setPosition(this->position);
	cam->setDirection(1, 0, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, 1.0f, 10.0f);
	cam->setPosition(this->position);
	cam->setDirection(-1, 0, 0);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, 1.0f, 10.0f);
	cam->setPosition(this->position);
	cam->setDirection(0, 0, 1);
	sides.push_back(cam);

	cam = new Camera(XM_PI * fov, 1.0f, 1.0f, 10.0f);
	cam->setPosition(this->position);
	cam->setDirection(0, 0, -1);
	sides.push_back(cam);

}
