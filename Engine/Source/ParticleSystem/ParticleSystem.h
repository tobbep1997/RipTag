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

	int nrOfEmitters;

	std::vector<ParticleEmitter*> m_ParticleEmitter;
	void Draw();

	void Reset();
};

