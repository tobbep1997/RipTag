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
	enum typeOfEmitter { FIRE, SMOKE };
	typeOfEmitter type = FIRE;

	std::vector<ParticleEmitter*> m_ParticleEmitter;
	void Draw();
	void Update(float timeDelata, Camera * camera);
	ParticleEmitter *CreateEmitter(DirectX::XMFLOAT3 spawnPos, typeOfEmitter type, float lifeTime);

	void Reset();
};

