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
	enum HUDTypeEnum
	{
		QuadType = 0U,
		CircleType = 1U
	};
	struct HUDTypeStruct
	{
		DirectX::XMFLOAT4A center;
		DirectX::XMUINT4 type;
	};
private:
	ID3D11SamplerState *	m_sampler;
	DirectX::SpriteBatch *	m_spriteBatch;

	ID3D11BlendState *		m_blendState;
	ID3D11DepthStencilState*	m_depthStencilState;

	ID3D11Buffer * m_HUDTypeBuffer;
	HUDTypeStruct m_HUDTypeValues;

public:
	Render2D();
	~Render2D();

	void Init();
	void GUIPass();
	void Release();

};

