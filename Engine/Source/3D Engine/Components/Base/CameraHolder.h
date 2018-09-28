#pragma once
#include "../Camera.h"

//Makes sure to call p_initCamera
class CameraHolder
{
protected:
	Camera * p_camera;

	void p_initCamera(Camera * camera);
public:
	CameraHolder();
	virtual~CameraHolder();

	Camera * getCamera() const;
};

