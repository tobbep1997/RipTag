#pragma once
#include <DirectXMath.h>

class Camera;
//Makes sure to call p_initCamera
class CameraHolder
{
private:

	float m_moveBob = 0.0f;
	float m_stopBob = 0.0f;
	float m_sprintBob = 0.0f;

	float m_moveFreq = 1.8f; 
	float m_moveAmp = 0.01f;

	float m_stopFreq = 0.7f; 
	float m_stopAmp = 0.03f;

	float m_sprintFreq = 1.4f; 
	float m_sprintAmp = 0.04f;

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

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

};

