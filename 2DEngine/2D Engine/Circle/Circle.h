#pragma once
#include "2D Engine/Quad/Quad.h"
class Circle : public Quad
{
	float m_radie = 0;
	float m_innerRadie = 0;

public:
	Circle();
	~Circle();

	bool setRadie(const float & radie);
	bool setInnerRadie(const float & innerRadie);

	unsigned int getType() const override;
	DirectX::XMFLOAT4 getCenter() const override;
	const float & getRadie() const override;
	const float & getInnerRadie() const override;

	void setAngle(const float & angle) override;
	const float & getAngle() const override;

};

