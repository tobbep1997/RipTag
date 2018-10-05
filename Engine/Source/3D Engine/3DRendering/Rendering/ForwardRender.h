#pragma once
#pragma warning (disable : 4267)
#include "../../../Shader/ShaderManager.h"
#include "../../Components/Camera.h"
#include "ShadowMap.h"
#include <thread>
#include "VisabilityPass/VisabilityPass.h"

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

	//Standard �
	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;

	D3D11_VIEWPORT				m_viewport;

	//Constant Buffer TEMP
	//TODO::Fixa constant buffers
	//Uppdatera bara 1 g�ng
	//Fixa en constant_buffer.hlsl

	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_cameraBuffer = nullptr;
	CameraBuffer m_cameraValues;

	ID3D11Buffer * m_lightBuffer = nullptr;
	LightBuffer m_lightValues;

	Animation::AnimationCBuffer m_animationBuffer;
	ShadowMap m_shadowMap;

	VisabilityPass m_visabilityPass;
	ID3D11Buffer* m_GuardBuffer;

	//LightCulling Related
	float m_lightCullingDistance = 100;	//Culling Distance for lights
	// after optimization, change this to 8
	float m_forceCullingLimit = 8;		//If there are more then lights left then the limit it will force cull it
	std::thread m_shaderThreads[3];
	bool m_firstRun = true;
	ID3D11BlendState* m_alphaBlend;

	ID3D11RasterizerState * m_standardRast;
	ID3D11RasterizerState * m_wireFrame;

public:
	ForwardRender();
	~ForwardRender();

	void Init(IDXGISwapChain*				swapChain,
		ID3D11RenderTargetView*		backBufferRTV,
		ID3D11DepthStencilView*		depthStencilView,
		ID3D11Texture2D*			depthBufferTex,
		ID3D11SamplerState*			samplerState,
		D3D11_VIEWPORT				viewport);


	void GeometryPass();
	void AnimatedGeometryPass();
	void Flush(Camera & camera);
	void Clear();
	void Present();

	void Release();
private:
	void _tempGuardFrustumDraw();

	void _simpleLightCulling(Camera & cam);

	void _createConstantBuffer();
	void _createSamplerState();
	void _mapObjectBuffer(Drawable * drawable);
	void _mapCameraBuffer(Camera & camera);
	void _mapSkinningBuffer(Drawable * drawable);
	void _mapLightInfoNoMatrix();



	//For visability

	//void _SetShaders(int i);
	void _setAnimatedShaders();
	void _setStaticShaders();

	//VisabilityPass
	void VisabilityPass();

	void _createShaders();
	void _createShadersInput();

	void _wireFramePass();

};
