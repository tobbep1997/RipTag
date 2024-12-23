#pragma once
#include <windows.h>
#include <DirectXMath.h>

#include "Base/Transform.h"

class Camera : public Transform
{
public:
	enum Perspectiv
	{
		Enemy,
		Player
	};
private:
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
	DirectX::XMFLOAT4A m_direction;
	DirectX::XMFLOAT4A m_UP;
	//-------------------------------------------------------------------------------------------	
	/*
		The nessesery varibles to create the projection matrix	
	*/
	float m_fov;
	float m_aspectRatio;
	float m_nearPlane, m_farPlane;
	bool m_usingDir = true;


	
	Perspectiv m_perspectiv;
public:
	Camera(float fov = DirectX::XM_PI * 0.5f, float aspectRatio = 16.0f/9.0f, float nearPlane = 0.1f, float farPlane = 10.0f);
	~Camera();
	
	//-------------------------------------------------------------------------------------------	
	/*
		Get and sets
	*/	
	
	/*
		The Translate functions add the input to the position instead of changing it
		This is relative to the forward vector
	*/
	void Translate(const DirectX::XMFLOAT4A & pos);
	void Translate(float x, float y, float z, float w = 1);
	/*
		The Rotate functions rotates the direction
		This is relative to the forward vector
	*/
	void Rotate(const DirectX::XMFLOAT4A & rotation);
	void Rotate(float x, float y, float z, float w = 0);

	void setDirection(const DirectX::XMFLOAT4A & direction);
	void setDirection(float x, float y, float z, float w = 0);

	void setLookTo(const DirectX::XMFLOAT4A & pos);
	void setLookTo(float x, float y, float z, float w = 0);

	void setUP(DirectX::XMFLOAT4A up);
	void setUP(float x, float y, float z, float w = 0);

	const DirectX::XMFLOAT4A & getUP() const;

	void setNearPlane(float nearPlane);
	void setFarPlane(float farPlane);

	void setFOV(float fov);

	const float & getFarPlane() const;
	const float & getNearPlane() const;
	const float & getFOV() const;

	const DirectX::XMFLOAT4A & getDirection() const;
	DirectX::XMFLOAT4A getRight() const;

	const DirectX::XMFLOAT4A getYRotationEuler();
	const DirectX::XMFLOAT4A getPitch();
	DirectX::XMFLOAT4A getForward() const;

	const DirectX::XMFLOAT4X4A & getView();
	const DirectX::XMFLOAT4X4A & getProjection() const;
	const DirectX::XMFLOAT4X4A & getViewProjection();
	DirectX::XMVECTOR getRotation();
	//-------------------------------------------------------------------------------------------	

	void setPerspectiv(Perspectiv perspectiv);
	const Perspectiv & getPerspectiv() const;
private:
	//-------------------------------------------------------------------------------------------	
	/*
		These create all the useful combinations of the matrixes
	*/
	void _calcViewMatrix(bool dir = true);
	void _calcProjectionMatrix();
	void _calcViewProjectionMatrix();
	//-------------------------------------------------------------------------------------------	
	/*
		Help Functions, this might be added to a static math class if more classes needs this
	*/
	DirectX::XMFLOAT4A _add(const DirectX::XMFLOAT4A & a, const DirectX::XMFLOAT4A & b);

public:
	DirectX::XMMATRIX ForceRotation(const DirectX::XMFLOAT4X4A& rotMatrix);
};