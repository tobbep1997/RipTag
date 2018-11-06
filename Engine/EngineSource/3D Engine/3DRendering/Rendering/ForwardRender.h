#pragma once
#pragma warning (disable : 4267)
#include <thread>
#include <d3d11_1.h>
#include <DirectXMath.h>


enum ObjectType;
enum EntityType;

struct StaticVertex;
struct DynamicVertex;

class Drawable;
class Camera;
class VisibilityComponent;
class PointLight;
class ShadowMap;
class Render2D;

class VisabilityPass;

namespace Animation
{
	class AnimationCBuffer;
};

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

	struct TextureBuffer
	{
		DirectX::XMINT4		usingTexture;
		DirectX::XMFLOAT4A	textureTileMult;
		DirectX::XMFLOAT4A	color;
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
	ID3D11SamplerState*			m_shadowSampler;
	

	D3D11_VIEWPORT				m_viewport;

	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_cameraBuffer = nullptr;
	CameraBuffer m_cameraValues;

	ID3D11Buffer * m_lightBuffer = nullptr;
	LightBuffer m_lightValues;

	ID3D11Buffer * m_textureBuffer = nullptr;
	TextureBuffer m_textureValues;

	Animation::AnimationCBuffer * m_animationBuffer;
	ShadowMap * m_shadowMap;
	Render2D * m_2DRender;


	VisabilityPass * m_visabilityPass;
	ID3D11Buffer* m_GuardBuffer;

	

	float m_lightCullingDistance = 100;	
	float m_forceCullingLimit = 7;		
	std::thread m_shaderThreads[3];
	bool m_firstRun = true;
	ID3D11BlendState* m_alphaBlend;

	ID3D11RasterizerState * m_standardRast;
	ID3D11RasterizerState * m_wireFrame;
	ID3D11RasterizerState * m_disableBackFace;

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

	void Release();
private:
	void _GuardFrustumDraw();

	void _simpleLightCulling(Camera & cam);
	void _GuardLightCulling();

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
	void _visabilityPass();

	void _createShaders();
	void _createShadersInput();

	void _wireFramePass();

	int run = 0;
};
