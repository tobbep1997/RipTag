#pragma once
#include "../3D Engine/Camera.h"
#include <vector>

class PointLight
{
public:
	enum ShadowDir
	{
		Y_POSITIVE = 0,
		Y_NEGATIVE = 1,
		X_POSITIVE = 2,
		X_NEGATIVE = 3,
		Z_POSITIVE = 4,
		Z_NEGATIVE = 5,
		XYZ_ALL = 6
	};

private:
	const float FOV = DirectX::XM_PI * 0.5f;
	
	std::vector<Camera *> m_sides;
	DirectX::XMFLOAT4A m_position;
	DirectX::XMFLOAT4A m_color;


	float m_nearPlane;
	float m_farPlane;
	float m_dropOff;

public:
	PointLight();
	~PointLight();

	void CreateShadowDirection(ShadowDir direction);

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
	
	void setNearPlane(float nearPlane);
	void setFarPlane(float farPlane);

	float GetDistanceFromCamera(Camera & camera);

private:
	void _createSides();
	void _createSide(const DirectX::XMFLOAT4A & dir, const DirectX::XMFLOAT4A & up);
	void _updateCameras();

};

