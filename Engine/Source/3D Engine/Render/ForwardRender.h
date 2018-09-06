#pragma once
#include "../../Shader/ShaderManager.h"

class ForwardRender
{

struct tempCPU
{
	float val1;
	float val2;
	float val3;
	float val4;

};

private:

	
	ID3D11VertexShader * m_vertexShader;
	ID3D11PixelShader * m_pixelShader;

	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;

	D3D11_VIEWPORT				m_viewport;

	//Constant Buffer TEMP
	ID3D11Buffer* m_tempConstant = nullptr;
	tempCPU m_values = { 0.0,0.0,0.0,0.0 };;

public:
	ForwardRender();
	~ForwardRender();

	void Init(	IDXGISwapChain*				m_swapChain,
				ID3D11RenderTargetView*		m_backBufferRTV,
				ID3D11DepthStencilView*		m_depthStencilView,
				ID3D11Texture2D*			m_depthBufferTex,
				ID3D11SamplerState*			m_samplerState,
				D3D11_VIEWPORT				m_viewport);
	
	void GeometryPass();
	void Flush();
	void Present();

	void Release();
private:
	void _CreateConstantBuffer();
	void _mapTempConstantBuffer();
};

