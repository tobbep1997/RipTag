#include "Button.h"


Button::Button(Transform2D * transform2D)
{
	this->m_transform2D = transform2D;
}

Button::~Button()
{
}

const bool Button::Inside(const DirectX::XMFLOAT2 & mousePos) const
{
	float x = mousePos.x;
	float y = 1.0f - mousePos.y;

	return x >= m_transform2D->getPosition().x && x <= m_transform2D->getPosition().x + m_transform2D->getSize().x && y >= m_transform2D->getPosition().y && y <= m_transform2D->getPosition().y + m_transform2D->getSize().y;
}
