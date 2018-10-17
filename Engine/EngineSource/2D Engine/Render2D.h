#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../3D Engine/3DRendering/Framework/DirectXRenderingHelpClass.h"
#include "Quad/Quad.h"


class Render2D
{
private:
	ID3D11SamplerState *	m_sampler;

	ID3D11Buffer *					m_positionBuffer;
	Transform2D::Transform2DBuffer	m_positionValues;

	ID3D11RasterizerState *			m_rasterizerState;
public:
	Render2D();
	~Render2D();

	void Init();
	void GUIPass();
	void Release();

private:
	void _mapBuffers(Quad * quad);
};

