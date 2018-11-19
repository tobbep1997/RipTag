#pragma once
#include "..\Triggers\Trigger.h"

class Torch : public Trigger
{
private:
	PointLight * pPointLight = nullptr;
	ParticleEmitter * pParticles = nullptr;
	Camera * pCamera = nullptr;
public:
	Torch(PointLight * pLight, ParticleEmitter * pParticleEmitter, int _uniqueId);
	~Torch();
	void Update(double deltaTime) override;
	void Draw() override;
	void QueueLight();
	void BeginPlay() override;

	PointLight * getPointLightPtr() { return pPointLight; }

	void setCamera(Camera * ptr) { this->pCamera = ptr; }
	void setUniqueID(int id) { Trigger::setUniqueId(id); }
private:
	void _playSound(AudioEngine::SoundType st = AudioEngine::Player) override;
};

