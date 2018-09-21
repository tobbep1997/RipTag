#pragma once
#pragma warning (disable : 4267)
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "../Extern.h"
#include "../../Shader/ShaderManager.h"



class ShadowMap
{
private:

struct ObjectBuffer
{
	DirectX::XMFLOAT4X4A worldMatrix;
};
struct PointLightBuffer
{
	DirectX::XMFLOAT4X4A viewProjection[8][6];
	DirectX::XMINT4 nrOfviewProjection[8];
	DirectX::XMINT4 nrOfLights;
};
struct LightCameraBuffer
{
	DirectX::XMFLOAT4X4A viewProjection;
};

private:
	const unsigned int RENDER_TARGET_VIEW_COUNT = 8 * 6;

	D3D11_VIEWPORT				m_shadowViewport;
	ID3D11SamplerState*			m_shadowSamplerState;
	ID3D11ShaderResourceView *	m_shadowShaderResourceView[6];
	ID3D11DepthStencilView*		m_shadowDepthStencilView;
	ID3D11Texture2D*			m_shadowDepthBufferTex;
	

	ID3D11RenderTargetView*		m_renderTargetView;
	ID3D11Texture2D*			m_renderTargetsTexture;

	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_lightMatrixBuffer = nullptr;
	LightCameraBuffer m_lightMatrixValues;

	ID3D11Buffer * m_allLightMatrixBuffer = nullptr;
	PointLightBuffer m_allLightMatrixValues;


public:
	ShadowMap();
	~ShadowMap();

	void Init(UINT width, UINT height);

	void ShadowPass();

	void MapAllLightMatrix(std::vector<PointLight*> * lights);

	void SetSamplerAndShaderResources();
	void Clear();
	void Release();
private:
	void _createShadowViewPort(UINT width, UINT height);
	void _createShadowDepthStencilView(UINT width, UINT hight);
	void _createBuffers();
	void _createRenderTargets(UINT width, UINT height);

	void _mapLightMatrix(PointLight * pointLight, unsigned int i);
	void _mapObjectBuffer(Drawable * drawable);
};

