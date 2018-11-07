#pragma once
#include "2D Engine/Quad/Quad.h"
class Circle : public Quad
{
	float m_radie = 0;

public:
	Circle();
	~Circle();

	bool setRadie(const float & radie);

	unsigned int getType() const override;
	DirectX::XMFLOAT4 getCenter() const override;
	const float & getRadie() const override;
};

