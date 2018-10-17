#pragma once
#include "../Camera.h"

//Makes sure to call p_initCamera
class CameraHolder
{

protected:
	Camera * p_camera;

	DirectX::XMFLOAT4A m_lastPeek;
	DirectX::XMFLOAT4A m_lastSideStep;
	float m_peekSpeed = 10.0f;

	void p_initCamera(Camera * camera);

	DirectX::XMFLOAT4A p_CameraTilting(double deltaTime, float targetPeek, const DirectX::XMFLOAT4A & pos );
public:
	CameraHolder();
	virtual~CameraHolder();

	Camera * getCamera() const;

};

