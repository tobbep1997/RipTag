#include "PointLight.h"
#include "../3D Engine/Extern.h"


PointLight::PointLight()
{
}


PointLight::~PointLight()
{
}

void PointLight::Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float dropOff)
{
	this->position = position;
	this->color = color;
	this->dropOff = dropOff;
}

const DirectX::XMFLOAT4A & PointLight::getPosition()
{
	return position;
}

const DirectX::XMFLOAT4A & PointLight::getColor()
{
	return color;
}

const std::vector<Camera>& PointLight::getSides()
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
