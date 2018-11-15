#include "Engine2DPCH.h"
#include "Circle.h"

DirectX::XMFLOAT2 Add(const DirectX::XMFLOAT2 & a, const DirectX::XMFLOAT2 & b)
{
	return DirectX::XMFLOAT2(a.x + b.x, a.y + b.y);
}

DirectX::XMFLOAT4 GetCenter(const DirectX::XMFLOAT2 & sum, const unsigned int size = 0U)
{
	return DirectX::XMFLOAT4(sum.x / size, sum.y / 2, 0, 0);
}

Circle::Circle()
{

}

Circle::~Circle()
{
}

bool Circle::setRadie(const float & radie)
{
	this->m_radie = radie >= .5f ? .5f : radie;
	return !(radie >= .5f);
}

bool Circle::setInnerRadie(const float & innerRadie)
{
	this->m_innerRadie = innerRadie <= 0.0f ? 0.0f : innerRadie;
	return !(innerRadie <= 0.0f);
}

unsigned int Circle::getType() const
{
	return 1U;
}

DirectX::XMFLOAT4 Circle::getCenter() const
{
	return GetCenter(Add(Add(Add(quadVertex[0].position, quadVertex[1].position), quadVertex[2].position), quadVertex[3].position), 4);
}

const float & Circle::getRadie() const
{
	return m_radie;
}

const float & Circle::getInnerRadie() const
{
	return m_innerRadie;
}

void Circle::setAngle(const float & angle)
{
	p_angle = DirectX::XMConvertToRadians(angle);
	while (p_angle > DirectX::XM_PI * 2.0)
	{
		p_angle -= DirectX::XM_PI * 2.0;
	}
}

const float& Circle::getAngle() const
{
	return this->p_angle;
}
