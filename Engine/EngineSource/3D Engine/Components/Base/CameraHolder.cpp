#include "CameraHolder.h"



void CameraHolder::p_initCamera(Camera * camera)
{
	this->p_camera = camera;
}

CameraHolder::CameraHolder()
{
}


CameraHolder::~CameraHolder()
{
	delete this->p_camera;
}

Camera * CameraHolder::getCamera() const
{
	return this->p_camera;
}
