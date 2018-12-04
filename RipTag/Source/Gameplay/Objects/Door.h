#pragma once
#include "../Triggers/Triggerable.h"

class Door : public Triggerable
{
private:
	DirectX::XMFLOAT4A m_closePos;
	DirectX::XMFLOAT4A m_openPos;
	DirectX::XMFLOAT3 m_startRotModel;
	DirectX::XMFLOAT3 m_startRotBox;
	float m_timer = 0.0f;
	bool m_wasClosed = false;

	AudioEngine::SoundDesc m_sd;
	FMOD::Channel * m_channel = nullptr;

public:
	Door();
	Door(int uniqueID, int linkedID, bool isTrigger);
	~Door();

	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ);
	void Update(double deltaTime) override;
};

