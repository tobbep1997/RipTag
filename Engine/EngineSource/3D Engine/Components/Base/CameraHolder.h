#pragma once
#include "../Camera.h"

//Makes sure to call p_initCamera
class CameraHolder
{
private:

	float m_sprintBob = 0.0f;
	float m_walkBob = 0.0f;
	
	float m_sprintingFreq = 1.9f;
	float m_sprintingBobAmp = 0.15f;

	float m_walkingFreq = 1.7f;
	float m_walkingBobAmp = 0.15f;

	float m_stopBobFreq = 1.5f;
	float m_stopBobAmp = 0.1f;

	float m_currentAmp = 0.0f; 

	float m_offset = 0.0f;

protected:
	Camera * p_camera;


	enum MoveState
	{
		Walking,
		Sprinting,
		Idle
	};
	MoveState p_moveState; 

	DirectX::XMFLOAT4A m_lastPeek = { 0,0,0,0 };
	DirectX::XMFLOAT4A m_lastSideStep = { 0,0,0,0 };
	float m_peekSpeed = 10.0f;

	float m_crouchAnimSteps = 0.0f;
	float m_crouchSpeed = 5.0f;
	float m_lastHeight = 0;

	void p_initCamera(Camera * camera);
	double p_viewBobbing(double deltaTime, double velocity, double moveSpeed, MoveState moveState);
	DirectX::XMFLOAT4A p_CameraTilting(double deltaTime, float targetPeek, const DirectX::XMFLOAT4A & pos );
	double p_Crouching(double deltaTime, float& startHeight, const DirectX::XMFLOAT4A & pos);
public:
	CameraHolder();
	virtual~CameraHolder();

	Camera * getCamera() const;

};

