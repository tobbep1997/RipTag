#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
class Camera
{
	DirectX::XMFLOAT4X4A m_view;
	DirectX::XMFLOAT4X4A m_projection;
	DirectX::XMFLOAT4X4A m_viewProjection;

	DirectX::XMFLOAT4A m_position;
	DirectX::XMFLOAT4A m_direction;
	DirectX::XMFLOAT4A m_UP;

	float m_fov;
	float m_aspectRatio;
	float m_nearPlane, m_farPlane;

	void _calcViewMatrix();
	void _calcProjectionMatrix();
	void _calcViewProjectionMatrix();
public:
	Camera(float fov = DirectX::XM_PI * 0.5f, float aspectRatio = 16.0f/9.0f, float nearPlane = 1.0f, float farPlane = 20.0f);
	~Camera();

	void setPosition(DirectX::XMFLOAT4A pos);
	void setPosition(float x, float y, float z, float w = 1);
	
	void setDirection(DirectX::XMFLOAT4A direction);
	void setDirection(float x, float y, float z, float w = 0);

	DirectX::XMFLOAT4X4A getView();
	DirectX::XMFLOAT4X4A getProjection();
	DirectX::XMFLOAT4X4A getViewProjection();
	
};