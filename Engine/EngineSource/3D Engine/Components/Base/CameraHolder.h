#pragma once
#include <DirectXMath.h>

class Camera;
//Makes sure to call p_initCamera
class CameraHolder
{
private:
	const float SPEED_CONSTANT = 1.5f;
	const float IDLE_CONSTANT = 1.0f;
	const float VERTICAL_AMPLIFIER = 0.1f;
	const float HORIZONTAL_AMPLIFIER = 0.05f;

	float m_verticalBob = 0.0f;
	bool m_verAdder = true;
	float m_horizontalBob = DirectX::XM_PIDIV2;
	bool m_horAdder = true;
	float m_offsetX = 0.0f;
	float m_offsetY = 0.0f;


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
	float m_peekSpeed = 5.0f;

	float m_crouchAnimSteps = 0.0f;
	float m_crouchSpeed = 5.0f;
	float m_lastHeight = 0;

	void p_initCamera(Camera * camera);
	double p_viewBobbing(double deltaTime, double moveSpeed, b3Body * owner);
	DirectX::XMFLOAT4A p_CameraTilting(double deltaTime, float targetPeek);
	double p_Crouching(double deltaTime, float& startHeight, const DirectX::XMFLOAT4A & pos);
public:
	CameraHolder();
	virtual~CameraHolder();
	DirectX::XMFLOAT4A getLastPeek()const ;
	void setLastPeek(DirectX::XMFLOAT4A peek);
	Camera * getCamera() const;

	float lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

};

