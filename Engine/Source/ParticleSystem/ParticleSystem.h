#pragma once
#include <filesystem>

class ParticleEmitter;

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	std::vector<ParticleEmitter*> m_ParticleEmitter;

	void Queue();
	void Update(float timeDelata, Camera * camera);

	void AddEmitter(ParticleEmitter * pEmitter);
	
	void clearEmitters();

private: 
	void _loadParticleShaders();
	void _loadParticleTextures();
};

