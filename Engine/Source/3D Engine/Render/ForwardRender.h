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

private:

	struct sortStruct
	{
		float distance;
		int lightBelong;
	};

	DirectX::XMFLOAT4X4A view;
	DirectX::XMFLOAT4X4A projection;

	
	ID3D11VertexShader * m_vertexShader;
	ID3D11PixelShader * m_pixelShader;

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
	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_cameraBuffer = nullptr;
	CameraBuffer m_cameraValues;

	ID3D11Buffer * m_lightBuffer = nullptr;
	LightBuffer m_lightValues;

	ShadowMap shadowMap;


	ID3D11Texture2D* m_uavTextureBuffer;		//IsReleased
	ID3D11Texture2D* m_uavTextureBufferCPU;		//IsReleased
	ID3D11UnorderedAccessView* m_visabilityUAV;	//IsReleased

	//LightCulling Related
 	float m_lightCullingDistance = 100;	//Culling Distance for lights
	// after optimization, change this to 8
	float m_forceCullingLimit = 2;		//If there are more then lights left then the limit it will force cull it
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
	void AnimatedGeometryPass(Camera & camera);
	void Flush(Camera & camera);
	void Present();

	void Release();
private:

	void _SimpleLightCulling(Camera & cam);

	void _CreateConstantBuffer();
	void _CreateSamplerState();
	void _mapObjectBuffer(Drawable * drawable);
	void _mapCameraBufferToVertex(Camera & camera);
	void _mapCameraBufferToPixel(Camera & camera);
	void _mapLightInfoNoMatrix();
	void CREATE_VIEWPROJ();


	//void _SetShaders(int i);
	void _SetAnimatedShaders();
	void _SetStaticShaders();

	void _createUAV();
};

