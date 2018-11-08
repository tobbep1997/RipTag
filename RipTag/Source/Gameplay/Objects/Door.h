#pragma once
#include "../Triggers/Triggerable.h"

class Door : public Triggerable
{
private:
	DirectX::XMFLOAT4A pos1;
	DirectX::XMFLOAT4A pos2;

public:
	Door();
	Door(int uniqueID, int linkedID, bool isTrigger);
	~Door();

	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ);
	void Update(double deltaTime) override;
};

