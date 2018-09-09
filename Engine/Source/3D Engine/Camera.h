#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
class Camera
{
	//-------------------------------------------------------------------------------------------	
	/*
		View and projection matrix
		m_viewProjection = projection x view
	*/
	DirectX::XMFLOAT4X4A m_view;
	DirectX::XMFLOAT4X4A m_projection;
	DirectX::XMFLOAT4X4A m_viewProjection;
	//-------------------------------------------------------------------------------------------	
	/*
		The nessesery varibles to create the view matrix
	*/
	DirectX::XMFLOAT4A m_position;
	DirectX::XMFLOAT4A m_direction;
	DirectX::XMFLOAT4A m_UP;
	//-------------------------------------------------------------------------------------------	
	/*
		The nessesery varibles to create the projection matrix	
	*/
	float m_fov;
	float m_aspectRatio;
	float m_nearPlane, m_farPlane;
	//-------------------------------------------------------------------------------------------	
	/*
		These create all the useful combinations of the matrixes
	*/
	void _calcViewMatrix(bool dir = true);
	void _calcProjectionMatrix();
	void _calcViewProjectionMatrix();
	//-------------------------------------------------------------------------------------------	
	bool m_usingDir = true;


public:
	Camera(float fov = DirectX::XM_PI * 0.5f, float aspectRatio = 16.0f/9.0f, float nearPlane = 0.1f, float farPlane = 50.0f);
	~Camera();
	//-------------------------------------------------------------------------------------------	
	/*
		Get and sets
	*/
	void setPosition(DirectX::XMFLOAT4A pos);
	void setPosition(float x, float y, float z, float w = 1);
	
	void setDirection(DirectX::XMFLOAT4A direction);
	void setDirection(float x, float y, float z, float w = 0);

	void setLookTo(DirectX::XMFLOAT4A pos);
	void setLookTo(float x, float y, float z, float w = 0);

	const DirectX::XMFLOAT4A & getPosition() const;
	const DirectX::XMFLOAT4A & getDirection() const;

	const DirectX::XMFLOAT4X4A & getView();
	const DirectX::XMFLOAT4X4A & getProjection() const;
	const DirectX::XMFLOAT4X4A & getViewProjection();
	//-------------------------------------------------------------------------------------------	

};