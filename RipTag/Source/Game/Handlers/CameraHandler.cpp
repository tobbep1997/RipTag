#include "RipTagPCH.h"
#include "CameraHandler.h"



Camera * CameraHandler::m_defultCamera;
Camera * CameraHandler::m_activeCamera;

CameraHandler::CameraHandler()
{
}

CameraHandler::~CameraHandler()
{
	delete m_defultCamera;
}

CameraHandler& CameraHandler::Instance()
{
	static CameraHandler init;
	m_activeCamera = nullptr;
	return init;
}

void CameraHandler::KillActiveCamera()
{
	m_activeCamera = nullptr;
}

void CameraHandler::setActiveCamera(Camera* cam)
{
	m_activeCamera = cam;
}

Camera* CameraHandler::getActiveCamera()
{
	if (m_activeCamera != nullptr)
	{
		return m_activeCamera;
	}
	
	return m_defultCamera;
	
}


