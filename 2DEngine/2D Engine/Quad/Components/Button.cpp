#include "Engine2DPCH.h"
#include "Button.h"


Button::Button(Transform2D * transform2D)
{
	this->m_transform2D = transform2D;
}

Button::~Button()
{
}

const bool Button::Inside(const DirectX::XMFLOAT2 & mousePos) 
{
	float x = mousePos.x;
	float y = 1.0f - mousePos.y;

	return x >= m_transform2D->getPosition().x - (m_transform2D->getSize().x / 4.0f) &&
		x <= m_transform2D->getPosition().x  + (m_transform2D->getSize().x / 4.0f) &&
		y >= m_transform2D->getPosition().y - (m_transform2D->getSize().y / 4.0f) &&
		y <= m_transform2D->getPosition().y  + (m_transform2D->getSize().y / 4.0f);
}
