#pragma once
#include "../../Shader/ShaderManager.h"

class ForwardRender
{

private:

	
	ID3D11VertexShader * m_vertexShader;
	ID3D11PixelShader * m_pixelShader;

	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;

	D3D11_VIEWPORT				m_viewport;



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

	
};

