#pragma once
#pragma warning (disable : 4244)
#include <windows.h>
#include <d3d11_1.h>
#include <DirectXMath.h>



enum ObjectType;
enum EntityType;

struct StaticVertex;
struct DynamicVertex;

class Drawable;
class VisibilityComponent;

namespace Animation {
	class AnimationCBuffer;
	class AnimationPlayer;
};

class VisabilityPass
{
private:
	const short int GUARD_RES_Y = 32;
	const short int GUARD_RES_X = (GUARD_RES_Y * (210.0f / 150.0f) + 0.5f);
	//Ration 210 / 150
	
	const wchar_t * DEPTH_PRE_PASS_STATIC_VERTEX_SHADER_PATH = L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertex.hlsl";
	const wchar_t * DEPTH_PRE_PASS_DYNAMIC_VERTEX_SHADER_PATH = L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertexAnimated.hlsl";
	const wchar_t * VISABILITY_PASS_PIXEL_SHADER_PATH = L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl";
	const wchar_t * STATIC_VERTEX_SHADER_PATH = L"../Engine/EngineSource/Shader/VertexShader.hlsl";
	const wchar_t * DYNAMIC_VERTEX_SHADER_PATH = L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl";

	ID3D11BlendState* m_alphaBlend; 


	struct GuardViewBuffer
	{
		DirectX::XMFLOAT4A cameraPosition;
		DirectX::XMFLOAT4X4A viewProjection;
	};
	struct ObjectBuffer
	{
		DirectX::XMFLOAT4X4A worldMatrix;
	};

	struct TextureBuffer
	{
		DirectX::XMINT4		usingTexture;
		DirectX::XMFLOAT4A	textureTileMult;
		DirectX::XMFLOAT4A	color;
	};

private:
	D3D11_VIEWPORT				m_guardViewPort;
	ID3D11Buffer *				m_guardViewBuffer;			//RE
	ID3D11DepthStencilView*		m_guardDepthStencil;		//RE
	ID3D11Texture2D*			m_guardDepthTex;			//RE
	ID3D11Texture2D*			m_guatdShaderResourceTex;	//RE	
	ID3D11RenderTargetView*		m_guardRenderTargetView;	//RE
	ID3D11ShaderResourceView*	m_guardShaderResource;		//RE
	ID3D11Buffer*				m_objectBuffer;				//RE

	ID3D11Buffer*				m_textureBuffer;			//RE
	TextureBuffer				m_textureValues;
public:
	VisabilityPass();
	~VisabilityPass();
	void Init();
	void GuardDepthPrePassFor(VisibilityComponent * target, ForwardRender * forwardRender, Animation::AnimationCBuffer * animBuffer = nullptr);
	void CalculateVisabilityFor(VisibilityComponent * target, Animation::AnimationCBuffer * animBuffer = nullptr);
	void SetViewportAndRenderTarget();
private:
	void _init();
	void _initViewPort();
	void _initViewBuffer();
	void _initObjectBuffer();
	void _initDSV();
	void _initSRV();
	
	void _initShaders();
	void _initVertexShaders();
	void _initPixelShaders();

	void _mapViewBuffer(VisibilityComponent * target);

	void _mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer);

	void _mapObjectBuffer(Drawable * target);

	void _drawForPlayer(Drawable * player, VisibilityComponent * enemy, int playerIndex);
};