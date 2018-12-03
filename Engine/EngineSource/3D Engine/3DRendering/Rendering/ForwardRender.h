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

	struct OutLineBuffer
	{
		DirectX::XMFLOAT4A outLineColor;
	};

	struct DestroyBuffer
	{
		DirectX::XMFLOAT4 timer;
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
	ID3D11DepthStencilState*	m_depthStencilState;
	ID3D11DepthStencilState*	m_particleDepthStencilState;
	

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

	ID3D11Buffer * m_destructionBuffer = nullptr;
	DestroyBuffer m_destroyBuffer;

	

	float m_lightCullingDistance = 50;	
	float m_forceCullingLimit = 5;		
	std::thread m_shaderThreads[3];
	bool m_firstRun = true;
	ID3D11BlendState* m_alphaBlend;

	ID3D11RasterizerState * m_standardRast;
	ID3D11RasterizerState * m_wireFrame;
	ID3D11RasterizerState * m_disableBackFace;
	ID3D11RasterizerState * m_NUKE;

	ID3D11Buffer * m_outlineBuffer;
	OutLineBuffer m_outLineValues;

	ID3D11DepthStencilState * m_write0State;
	ID3D11DepthStencilState * m_write1State;
	ID3D11DepthStencilState * m_OutlineState;
	ID3D11DepthStencilState * m_NUKE2;

	int shadowRun = 0;
	
public:
	ForwardRender();
	~ForwardRender();

	void Init(IDXGISwapChain*				swapChain,
		ID3D11RenderTargetView*		backBufferRTV,
		ID3D11DepthStencilView*		depthStencilView,
		ID3D11DepthStencilState*	m_depthStencilState,
		ID3D11Texture2D*			depthBufferTex,
		ID3D11SamplerState*			samplerState,
		D3D11_VIEWPORT				viewport,
		const WindowContext &		windowContext);


	void GeometryPass(Camera & camera);
	void PrePass(Camera & camera);

	void AnimationPrePass(Camera & camera);
	void AnimatedGeometryPass(Camera & camera);
	void Flush(Camera & camera);
	void Clear();

	void Release();
	void DrawInstanced(Camera * camera, std::vector<DX::INSTANCING::GROUP> * instanceGroup, const bool & bindTextures = true);
	void DrawInstancedCull(Camera * camera, const bool & bindTextures = true);

	DirectX::BoundingFrustum * _createBoundingFrustrum(Camera * camera);//Make sure to delete the pointer
	bool _Cull(DirectX::BoundingFrustum* camera, DirectX::BoundingBox* box);//Return true if the box sould be culled

private:


	void _GuardFrustumDraw();
	void _DBG_DRAW_CAMERA(Camera & camera);

	void _simpleLightCulling(Camera & cam);
	void _GuardLightCulling();

	void _createConstantBuffer();
	void _createSamplerState();
	void _mapObjectBuffer(Drawable * drawable);


	void _mapObjectOutlineBuffer(Drawable * drawable, const DirectX::XMFLOAT4A & pos);
	void _mapObjectInsideOutlineBuffer(Drawable * drawable, const DirectX::XMFLOAT4A & pos);

	void _mapCameraBuffer(Camera & camera);
	void _mapSkinningBuffer(Drawable * drawable);
	void _mapLightInfoNoMatrix();

	void _OutliningPass(Camera & cam);
	void _OutlineDepthCreate();

	//For visability

	//void _SetShaders(int i);
	void _setAnimatedShaders();
	void _setStaticShaders();

	//VisabilityPass
	void _visabilityPass();

	//ParticlePass
	void _particlePass(Camera * camera);

	void _createShaders();
	void _createShadersInput();

	void _wireFramePass(Camera * camera);


	int run = 0;
};
