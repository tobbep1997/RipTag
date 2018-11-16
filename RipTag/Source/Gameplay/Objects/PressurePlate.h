#pragma once
#include "../Triggers/Trigger.h"

class PressurePlate : public Trigger
{
private:
	
public:
	PressurePlate();
	PressurePlate(int uniqueId, int linkedID, bool isTrigger);
	~PressurePlate();
		
	void Init(float xPos, float yPos, float zPos, float pitch, float yaw, float roll, float bboxScaleX, float bboxScaleY, float bboxScaleZ, float scaleX, float scaleY, float scaleZ);
	void Update(double dt);
private:
	void _playSound(AudioEngine::SoundType st = AudioEngine::Player) override;
};

