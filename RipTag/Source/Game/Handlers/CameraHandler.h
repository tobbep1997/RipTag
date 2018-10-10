#pragma once
#include "../Engine/Source/3D Engine/Components/Camera.h"
//-----------------------
/*
 * Instructions on how to use
 * You have to instance the class in somewhere early, before the first draw call
 * The camera handler comes with a defult camera
 * when you want to swap the camera just use 
 * setActiveCamera(cam);
 * when you wan to kill it use 
 * KillActiveCamera();
 * 
 * getActiveCamera will be used by the renderingManager
 * in the Flush
 * 
 * TaDa
 */

class CameraHandler
{
private:
	static Camera m_defultCamera;

	static Camera * m_activeCamera;

	CameraHandler();
	~CameraHandler();
public:

	static CameraHandler& Instance();

	//This only sets the camera to nullptr, you have to delete it
	static void KillActiveCamera();
	static void setActiveCamera(Camera * cam);
	static Camera * getActiveCamera();
private:

};
