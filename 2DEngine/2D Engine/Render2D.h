#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

namespace DirectX {
	class SpriteBatch;
};

class Quad;


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

