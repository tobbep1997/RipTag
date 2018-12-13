#pragma once
#include "../../IRender.h"

class Player;

class SkyBox : IRender
{
private:
	struct SkyboxBuffer
	{
		DirectX::XMFLOAT4X4A ViewProjection;
	};

private:
	SkyboxBuffer			m_skyboxValues;

	ID3D11Buffer *			m_skyboxBuffer;	
	ID3D11RasterizerState * m_rasterizerState;

	ID3D11InputLayout *		m_inputLayout;
	ID3D11VertexShader *	m_vertexShader;
	ID3D11PixelShader *		m_pixelShader;

public:
	SkyBox();
	~SkyBox();
	
	HRESULT Init(ForwardRender* forwardRender) override;
	HRESULT Release() override;
	HRESULT Draw(Drawable* drawable, const Camera & camera) override;
	HRESULT Draw(std::vector<DX::INSTANCING::GROUP> group, const Camera & camera) override;

private:
	void _createViewProjection(const Camera & camera);
};


