#pragma once
#pragma warning (disable : 4267)
#include "../../Shader/ShaderManager.h"
#include "../Camera.h"
#include "ShadowMap.h"
#include <thread>

class ForwardRender
{

struct ObjectBuffer
{
	DirectX::XMFLOAT4X4A worldMatrix;
};

struct CameraBuffer
{
	DirectX::XMFLOAT4A cameraPosition;
	DirectX::XMFLOAT4X4A viewProjection;
};

struct PointLightBuffer
{
	DirectX::XMFLOAT4X4A viewProjection[6];
};

struct LightBuffer
{
	DirectX::XMINT4		info;
	DirectX::XMFLOAT4A	dropOff[8];
	DirectX::XMFLOAT4A	position[8];
	DirectX::XMFLOAT4A	color[8];
};

struct GuardBuffer
{
	DirectX::XMFLOAT4X4A viewProj;
	DirectX::XMFLOAT4X4A viewProjInverse;
	DirectX::XMFLOAT4X4A worldMatrix;
};



private:

	struct sortStruct
	{
		float distance;
		int lightBelong;
	};
	//ShaderTest
	std::wstring m_lastVertexPath;
	std::wstring m_lastPixelPath;

	//Standard §
	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;

	D3D11_VIEWPORT				m_viewport;

	//Constant Buffer TEMP
	//TODO::Fixa constant buffers
	//Uppdatera bara 1 gång
	//Fixa en constant_buffer.hlsl

	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_cameraBuffer = nullptr;
	CameraBuffer m_cameraValues;

	ID3D11Buffer * m_lightBuffer = nullptr;
	LightBuffer m_lightValues;

	ID3D11Buffer* m_GuardBuffer;

	ShadowMap m_shadowMap;

	//ConstBuffer for visability

	ID3D11Texture2D* m_uavTextureBuffer;		//IsReleased
	ID3D11Texture2D* m_uavTextureBufferCPU;		//IsReleased
	//ID3D11Texture2D* m_uavKILLER;				//IsReleased
	ID3D11UnorderedAccessView* m_visabilityUAV;	//IsReleased

	ID3D11VertexShader * m_visaVertexShader;
	ID3D11PixelShader * m_visaPixelShader;
	//int lazyShit = 0;

	//LightCulling Related
 	float m_lightCullingDistance = 100;	//Culling Distance for lights
	// after optimization, change this to 8
	float m_forceCullingLimit = 8;		//If there are more then lights left then the limit it will force cull it
	std::thread m_shaderThreads[3];
	bool m_firstRun = true;

	const short int m_guardWH = 32;
	D3D11_VIEWPORT m_guardViewPort;

	ID3D11DepthStencilView*		m_guardPreDepthStencil;
	ID3D11Texture2D*			m_guardPreDepthTex;
	ID3D11Buffer*				m_objectBuffer2 = nullptr;
	ID3D11ShaderResourceView *	m_guardShaderResource;
	ID3D11BlendState* m_alphaBlend;

public:
	ForwardRender();
	~ForwardRender();

	void Init(	IDXGISwapChain*				swapChain,
				ID3D11RenderTargetView*		backBufferRTV,
				ID3D11DepthStencilView*		depthStencilView,
				ID3D11Texture2D*			depthBufferTex,
				ID3D11SamplerState*			samplerState,
				D3D11_VIEWPORT				viewport);
	

	void GeometryPass(Camera & camera);
	void AnimatedGeometryPass(Camera & camera);
	void Flush(Camera & camera);
	void Clear();
	void Present();
	
	void Release();
private:

	void _guardDepthStencil();
	void _guardDepthPrePass(Guard * guard);
	void _calcVisabilityFor(Guard * guard);
	void _tempGuardFrustumDraw();

	void _simpleLightCulling(Camera & cam);

	void _createConstantBuffer();
	void _createSamplerState();
	void _mapObjectBuffer(Drawable * drawable);
	void _mapCameraBufferToVertex(Camera & camera);
	void _mapCameraBufferToPixel(Camera & camera);
	void _mapLightInfoNoMatrix();


	//For visability

	//void _SetShaders(int i);
	void _setAnimatedShaders();
	void _setStaticShaders();

	//VisabilityPass
	void VisabilityPass();

	void _createUAV();

	void _createShaders();
	void _createShadersInput();
};

