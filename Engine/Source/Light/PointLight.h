#pragma once
#include "../3D Engine/Camera.h"
#include <vector>

class PointLight
{
private:

	std::vector<Camera> sides;
	DirectX::XMFLOAT4A position;
	DirectX::XMFLOAT4A color;
	float dropOff;
public:
	PointLight();
	~PointLight();

	void Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float dropOff = 1.0f);

	const DirectX::XMFLOAT4A & getPosition();
	const DirectX::XMFLOAT4A & getColor();
	const std::vector<Camera> & getSides();
	const float & getDropOff();

	void QueueLight();

};

