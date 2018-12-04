#include "EnginePCH.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	nrOfEmitters = 0;

}


ParticleSystem::~ParticleSystem()
{
	clearEmitters();

	for (int i = 0; i < nrOfEmitters; i++)
	{
		m_ParticleEmitter[i]->Clear();
	}	
}

void ParticleSystem::Queue()
{
	for (int i = 0; i < nrOfEmitters; i++)
	{
		m_ParticleEmitter[i]->Queue();
	}
}

void ParticleSystem::Update(float timeDelata, Camera * camera)
{
	for (int i = 0; i < nrOfEmitters; i++)
	{
		m_ParticleEmitter[i]->Update(timeDelata, camera);
	}
}

ParticleEmitter *ParticleSystem::CreateEmitter(DirectX::XMFLOAT3 spawnPos, typeOfEmitter type, float lifeTime)
{
	ParticleEmitter * newEmitter = new ParticleEmitter(type);
	newEmitter->setPosition(spawnPos.x, spawnPos.y, spawnPos.z);
	newEmitter->type = type;
	newEmitter->setEmmiterLife(lifeTime);
	m_ParticleEmitter.push_back(newEmitter);
	nrOfEmitters += 1;
	return newEmitter;
}

void ParticleSystem::clearEmitters()
{
	for (int i = 0; i < nrOfEmitters; i++)
	{
		if (m_ParticleEmitter[i])
		{
			delete m_ParticleEmitter[i];
			m_ParticleEmitter[i] = nullptr;
			nrOfEmitters = 0;
		}
	}
}
