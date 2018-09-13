#pragma once
#include "../3D Engine/Camera.h"
#include <vector>

class PointLight
{
private:

	std::vector<Camera *> sides;
	DirectX::XMFLOAT4A position;
	DirectX::XMFLOAT4A color;
	float dropOff;
public:
	PointLight();
	~PointLight();

	void Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float intencsity = 1.0f);

	const DirectX::XMFLOAT4A & getPosition();
	const DirectX::XMFLOAT4A & getColor();
	const std::vector<Camera *> & getSides();
	const float & getDropOff();

	void QueueLight();

	void SetPosition(const DirectX::XMFLOAT4A & pos);
	void SetPosition(float x, float y, float z, float w = 1);

	void SetColor(const DirectX::XMFLOAT4A & color);
	void SetColor(float x, float y, float z, float w = 1);

	void SetIntensity(float intencsity);
	


private:
	void _createSides();
	void _updateCameras();

};

