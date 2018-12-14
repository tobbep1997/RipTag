#pragma once
#include "..\Triggers\Trigger.h"

class Torch : public Trigger
{
private:
	PointLight * pPointLight = nullptr;
	ParticleEmitter * pParticles = nullptr;
	Camera * pCamera = nullptr;
	bool m_interacted = false;
	bool m_preState = false;
	FMOD::Channel * m_channel;
	AudioEngine::SoundDesc m_tourchSound;

	bool m_draw = true;

public:
	Torch(PointLight * pLight, ParticleEmitter * pParticleEmitter, int _uniqueId);
	~Torch();
	void Update(double deltaTime) override;
	void Draw() override;
	void QueueLight();
	void BeginPlay() override;

	PointLight * getPointLightPtr() { return pPointLight; }

	void DrawTorch(const bool & draw);

	void setCamera(Camera * ptr) { this->pCamera = ptr; }
	void setUniqueID(int id) { Trigger::setUniqueId(id); }
	void handleContact(RayCastListener::RayContact& contact);
	void Interact();
private:
	void _playSound(AudioEngine::SoundDesc * soundDesc) override;
};

