#pragma once
#include <d3d11.h>
#include <d3dcsx.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <time.h>

class ParticleEmitter;

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	std::vector<ParticleEmitter*> m_ParticleEmitter;
	ParticleEmitter *p = new ParticleEmitter;

	void Reset();
};

