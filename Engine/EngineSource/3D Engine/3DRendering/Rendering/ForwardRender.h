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
		DirectX::XMFLOAT4A TimerAndForwardVector;
		DirectX::XMFLOAT4X4A lastPos;
		DirectX::XMFLOAT4X4A worldMatrixInverse;
		DirectX::XMFLOAT4X4A worldMatrix;
		DirectX::XMINT4 typeOfAbility;
	};
	struct LerpableWorldPosBuffer
	{
		DirectX::XMFLOAT4X4A lastPos;
		DirectX::XMFLOAT4A timer;
		DirectX::XMINT4 typeOfAbility;
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
	IDXGISwapChain*				m_swapChain;			//Released engine3D
	ID3D11RenderTargetView*		m_backBufferRTV;		//Released engine3D
	ID3D11DepthStencilView*		m_depthStencilView;		//Released engine3D
	ID3D11Texture2D*			m_depthBufferTex;		//Released engine3D
	ID3D11SamplerState*			m_samplerState;			//Released engine3D
	ID3D11SamplerState*			m_shadowSampler;		//Released engine3D
	ID3D11DepthStencilState*	m_depthStencilState;	//Released engine3D
	ID3D11DepthStencilState*	m_particleDepthStencilState; //Released Forward
	

	D3D11_VIEWPORT				m_viewport;

	ID3D11Buffer* m_objectBuffer = nullptr;				   //RELEASED
	ObjectBuffer m_objectValues;						   //RELEASED
														   //RELEASED
	ID3D11Buffer * m_cameraBuffer = nullptr;			   //RELEASED
	CameraBuffer m_cameraValues;						   //RELEASED
														   //RELEASED
	ID3D11Buffer * m_lightBuffer = nullptr;				   //RELEASED
	LightBuffer m_lightValues;							   //RELEASED
														   //RELEASED
	ID3D11Buffer * m_textureBuffer = nullptr;			   //RELEASED
	TextureBuffer m_textureValues;

	Animation::AnimationCBuffer * m_animationBuffer;
	ShadowMap * m_shadowMap;	//Released
	Render2D * m_2DRender;		//Released


	VisabilityPass * m_visabilityPass;
	ID3D11Buffer* m_GuardBuffer;	//RElEASED 

	ID3D11Buffer * m_destructionBuffer = nullptr;
	DestroyBuffer m_destroyBuffer;

	ID3D11Buffer * m_lerpablePosBuffer = nullptr;
	LerpableWorldPosBuffer m_lerpablePosBufferInfo;

	

	float m_lightCullingDistance = 50;	
	float m_forceCullingLimit = 5;		
	std::thread m_shaderThreads[3];
	bool m_firstRun = true;
	ID3D11BlendState* m_alphaBlend;

	ID3D11RasterizerState * m_standardRast;		//RE
	ID3D11RasterizerState * m_wireFrame;		//RE
	ID3D11RasterizerState * m_disableBackFace;	//RE
	ID3D11RasterizerState * m_NUKE;				//RE

	ID3D11Buffer * m_outlineBuffer;				//RE
	OutLineBuffer m_outLineValues;

	ID3D11DepthStencilState * m_write0State;	//RE
	ID3D11DepthStencilState * m_write1State;	//RE
	ID3D11DepthStencilState * m_OutlineState;	//RE
	ID3D11DepthStencilState * m_NUKE2;			//RE

	int shadowRun = 0;

	ID3D11RenderTargetView * m_screenShootRender;
	ID3D11Texture2D * m_screenShootTex;
	ID3D11ShaderResourceView * m_screenShootSRV;

	struct BufferMapping
	{
		ID3D11Buffer * m_bufferVec = nullptr;
		BOOL m_occupied = FALSE;
	};

	std::vector<BufferMapping> m_bufferVec;
	UINT m_occpidePid = 0;
	UINT m_bufferSize = 100000;
	UINT m_bufferLenght = 70;
	
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
	void DrawInstancedCull(Camera * camera, const bool & bindTextures = true, bool GeoPass = false, bool PrePass = false);

	ID3D11BlendState* getAlphaBlendState(); 
	ID3D11DepthStencilState* getDepthStencilState(); 

	DirectX::BoundingFrustum * _createBoundingFrustrum(Camera * camera);//Make sure to delete the pointer
	bool _Cull(DirectX::BoundingFrustum* camera, DirectX::BoundingBox* box);//Return true if the box sould be culled

	void FlushScreenShoot(Camera & camera);
private:
	void _GeometryPassToPic(Camera & camera);
	void _AnimatedGeometryToPic(Camera & camera);

	void _InitScreenShoot();

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

	//VisabilityPass
	void _visabilityPass();

	//ParticlePass
	void _particlePass(Camera * camera);

	void _createShaders();
	void _createShadersInput();

	void _wireFramePass(Camera * camera);


	int run = 0;
};
