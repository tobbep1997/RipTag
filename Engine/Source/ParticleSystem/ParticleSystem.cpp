#include "EnginePCH.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	nrOfEmitters = 0;

}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Draw()
{
	for (int i = 0; i < nrOfEmitters; i++)
	{
		m_ParticleEmitter[i]->Draw();
	}
}

void ParticleSystem::Reset()
{
}
