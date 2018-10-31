#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "EngineSource/3D Engine/3DRendering/Framework/DirectXRenderingHelpClass.h"
#include "Quad/Quad.h"
#include "DirectXTK/SpriteBatch.h"


class Render2D
{
private:
	ID3D11SamplerState *	m_sampler;
	DirectX::SpriteBatch *	m_spriteBatch;

	ID3D11BlendState *		m_blendState;

public:
	Render2D();
	~Render2D();

	void Init();
	void GUIPass();
	void Release();

};

