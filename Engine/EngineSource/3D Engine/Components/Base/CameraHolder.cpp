#include "CameraHolder.h"



void CameraHolder::p_initCamera(Camera * camera)
{
	this->p_camera = camera;
}

double CameraHolder::p_viewBobbing(double deltaTime, double velocity, double moveSpeed)
{
	if (velocity != 0)
	{
		if (m_currentAmp < walkingBobAmp)
		{
			m_currentAmp += 0.0003f;
		}
		walkBob += (velocity * (moveSpeed * deltaTime));
		m_offset = walkingBobAmp * sin(freq*walkBob);
	}
	else
	{
		walkBob += 0.009f;

		if (m_currentAmp > stopBobAmp)
		{
			m_currentAmp -= 0.0001f;
		}
		m_offset = m_currentAmp * sin(stopBobFreq * walkBob);
	}
	return m_offset;
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
