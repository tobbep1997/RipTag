#pragma once
#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "Source/Physics/Wrapper/PhysicsComponent.h"

class Camera;
class b3Body;
class RayCastListener;

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
	const unsigned int SHADOW_SIDES = 6U;
	const float FOV = DirectX::XM_PI * 0.5f;
	struct TourchEffectVars
	{
		double timer;
		DirectX::XMFLOAT2 current, target;
		float ran;
	};

	TourchEffectVars m_tev;

	std::vector<Camera *>	m_sides;
	DirectX::XMFLOAT4A		m_position;
	DirectX::XMFLOAT4A		m_color;


	float m_nearPlane;
	float m_farPlane;
	float m_dropOff, m_intensity, m_pow;

	ID3D11ShaderResourceView *	m_shadowShaderResourceView;
	ID3D11DepthStencilView*		m_shadowDepthStencilView;
	ID3D11Texture2D*			m_shadowDepthBufferTex;

	PhysicsComponent m_phys;

	bool m_update = false;
	bool m_firstRun = true;

	BOOL m_useSides[6];

	float m_cullingDistanceToCamera = -0.0f;

	bool m_lightOn = true;

public:
	PointLight();
	PointLight(float * translation, float * color, float intensity);
	~PointLight();

	void CreateShadowDirection(ShadowDir direction);

	void Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float intencsity = 1.0f);

	void QueueLight();

	void setPosition(const DirectX::XMFLOAT4A & pos);
	void setPosition(float x, float y, float z, float w = 1);

	void setColor(const DirectX::XMFLOAT4A & color);
	void setColor(float x, float y, float z, float w = 1);

	void setIntensity(float intencsity);
	void setPower(float pow);
	void setDropOff(float dropOff);
	
	void setNearPlane(float nearPlane);
	void setFarPlane(float farPlane);
	
	float getDistanceFromCamera(Camera & camera);
	float getDistanceFromObject(const DirectX::XMFLOAT4A & oPos);
	const DirectX::XMFLOAT4A & getPosition() const;
	const DirectX::XMFLOAT4A & getColor() const;
	const std::vector<Camera *> & getSides() const;
	const float & getDropOff() const;
	const float & getPow() const;
	const float & getIntensity() const;

	const float & getFarPlane() const;
	const float & getFOV() const;

	void CreateShadowDirection(const std::vector<ShadowDir> & shadowDir);

	float TourchEffect(double deltaTime, float base, float amplitude);

	ID3D11ShaderResourceView * getSRV() const;
	ID3D11DepthStencilView * getDSV() const;
	ID3D11Texture2D * getTEX() const;

	void EnableSides(ShadowDir dir);
	void DisableSides(ShadowDir dir);
	//std::vector<Camera *>* getSides();

	void setUpdate(const bool & update);
	bool getUpdate() const;
	void FirstRun();
	
	void Clear();

	const BOOL * useSides() const;

	void RayTrace(b3Body & object, RayCastListener * rcl);
	DirectX::XMFLOAT4A getDir(b3Body & object) const;

	void	setDistanceToCamera(const float & distance);
	float	getDistanceToCamera() const;

	bool getLightOn() const;
	void setLightOn(bool bo);
	void SwitchLightOn();
private:
	void _createSides();
	void _createSide(const DirectX::XMFLOAT4A & dir, const DirectX::XMFLOAT4A & up);
	void _updateCameras();
	void _initDirectX(UINT width = 64U, UINT hight = 64U);
};

