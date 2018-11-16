#pragma once
#include "../Triggers/Triggerable.h"
class Bars : public Triggerable
{
private:
	DirectX::XMFLOAT4A m_closePos;
	DirectX::XMFLOAT4A m_openPos;
public:
	Bars();
	Bars(int uniqueID, int linkedID, bool isTrigger);
	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ);

	void Update(double deltaTime) override;

	~Bars();
};

