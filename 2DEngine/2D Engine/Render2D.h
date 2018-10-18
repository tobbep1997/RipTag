#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "EngineSource/3D Engine/3DRendering/Framework/DirectXRenderingHelpClass.h"
#include "Quad/Quad.h"


class Render2D
{
private:
	ID3D11SamplerState *	m_sampler;

public:
	Render2D();
	~Render2D();

	void Init();
	void GUIPass();
	void Release();

};

