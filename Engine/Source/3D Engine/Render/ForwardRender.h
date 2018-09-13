#pragma once
#include "../../Shader/ShaderManager.h"
#include "../Camera.h"
#include "ShadowMap.h"

class ForwardRender
{

struct ObjectBuffer
{
	DirectX::XMFLOAT4X4A worldMatrix;
};

struct CameraBuffer
{
	DirectX::XMFLOAT4X4A viewProjection;
};

struct PointLightBuffer
{
	DirectX::XMFLOAT4X4A viewProjection[6];
};

struct LightBuffer
{
	DirectX::XMINT4		info;
	DirectX::XMFLOAT4A	position[8];
	DirectX::XMFLOAT4A	color[8];
	float				dropOff[8];
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
	/*
	D3D11_VIEWPORT				m_shadowViewport;
	ID3D11SamplerState*			m_shadowSamplerState;
	ID3D11ShaderResourceView *	m_shadowShaderResourceView[6];
	ID3D11DepthStencilView*		m_shadowDepthStencilView[6];
	ID3D11Texture2D*			m_shadowDepthBufferTex[6];
	*/
	//Constant Buffer TEMP
	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_cameraBuffer = nullptr;
	CameraBuffer m_cameraValues;

	ID3D11Buffer * m_lightBuffer = nullptr;
	LightBuffer m_lightValues;

	ShadowMap shadowMap;

public:
	ForwardRender();
	~ForwardRender();

	void Init(	IDXGISwapChain*				m_swapChain,
				ID3D11RenderTargetView*		m_backBufferRTV,
				ID3D11DepthStencilView*		m_depthStencilView,
				ID3D11Texture2D*			m_depthBufferTex,
				ID3D11SamplerState*			m_samplerState,
				D3D11_VIEWPORT				m_viewport);
	

	void GeometryPass(Camera & camera);
	void Flush(Camera & camera);
	void Present();

	void Release();
private:


	void _CreateConstantBuffer();
	void _mapObjectBuffer(Drawable * drawable);
	void _mapCameraBuffer(Camera & camera);
	void _mapLightInfoNoMatrix();
	void CREATE_VIEWPROJ();


	void _SetShaders(int i);
};

