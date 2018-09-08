#pragma once
#include "../../Shader/ShaderManager.h"

class ForwardRender
{

struct tempCPU
{
	DirectX::XMFLOAT4X4A view;
	DirectX::XMFLOAT4X4A projection;

	DirectX::XMFLOAT4X4A worldMatrix;

};

private:

	DirectX::XMFLOAT4X4A view;
	DirectX::XMFLOAT4X4A projection;

	
	ID3D11VertexShader * m_vertexShader;
	ID3D11PixelShader * m_pixelShader;

	//ShaderTest
	std::wstring m_lastVertexPath;
	std::wstring m_lastPixelPath;

	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;

	D3D11_VIEWPORT				m_viewport;

	//Constant Buffer TEMP
	ID3D11Buffer* m_tempConstant = nullptr;
	tempCPU m_values;// = { 0.0,0.0,0.0,0.0 };;

public:
	ForwardRender();
	~ForwardRender();

	void Init(	IDXGISwapChain*				m_swapChain,
				ID3D11RenderTargetView*		m_backBufferRTV,
				ID3D11DepthStencilView*		m_depthStencilView,
				ID3D11Texture2D*			m_depthBufferTex,
				ID3D11SamplerState*			m_samplerState,
				D3D11_VIEWPORT				m_viewport);
	
	
	void Flush();
	//void Present();

	void Release();
private:
	void _GeometryPass();

	void _CreateConstantBuffer();
	void _mapTempConstantBuffer();
	void CREATE_VIEWPROJ();


	void _SetShaders(int i);
};

