#pragma once
#include <d3d11.h>
#include <d3dcsx.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <time.h>
#include "ParticleSystem.h"

class ParticleManager
{
public:
	ParticleManager();
	~ParticleManager();

	std::vector<ParticleSystem*> m_ParticleSystem;
	ID3D11Buffer* particleVertexBuffer;
};

