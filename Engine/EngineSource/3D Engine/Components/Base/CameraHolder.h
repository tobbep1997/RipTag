#pragma once
#include "../Camera.h"

//Makes sure to call p_initCamera
class CameraHolder
{
private:

	float m_lerpingStep = 0.1f; 

	float m_sprintBob = 0.0f;
	float m_walkBob = 0.0f;
	float m_stopBob = 0.0f;
	
	float m_sprintingFreq = 2.0f;
	float m_sprintingBobAmp = 0.3f;

	float m_walkingFreq = 3.0f;
	float m_walkingBobAmp = 0.2f;

	float m_stopBobFreq = 2.0f;
	float m_stopBobAmp = 0.15f;

	float m_currentAmp = m_stopBobAmp;
	float m_currentFreq = m_stopBobFreq; 

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

