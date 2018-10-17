#pragma once
#include "../Camera.h"

//Makes sure to call p_initCamera
class CameraHolder
{
private:

	float walkBob = 0.0f;
	float m_offset = 0.0f;
	float freq = 1.9f;
	float walkingBobAmp = 0.06f;
	float stopBobAmp = 0.010f;
	float stopBobFreq = 1.9f;

	float m_currentAmp = 0.0f;

protected:
	Camera * p_camera;

	DirectX::XMFLOAT4A m_lastPeek = { 0,0,0,0 };
	DirectX::XMFLOAT4A m_lastSideStep = { 0,0,0,0 };
	float m_peekSpeed = 10.0f;

	void p_initCamera(Camera * camera);
	double p_viewBobbing(double deltaTime, double velocity, double moveSpeed);
	DirectX::XMFLOAT4A p_CameraTilting(double deltaTime, float targetPeek, const DirectX::XMFLOAT4A & pos );
public:
	CameraHolder();
	virtual~CameraHolder();

	Camera * getCamera() const;

};

