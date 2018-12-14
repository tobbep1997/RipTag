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

const unsigned short int MAX_GUARDS = 5U;
class VisabilityPass : public IRender
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
	ID3D11Buffer *				m_guardViewBuffer[MAX_GUARDS];			//RE
	ID3D11DepthStencilView*		m_guardDepthStencil[MAX_GUARDS];		//RE
	ID3D11Texture2D*			m_guardDepthTex[MAX_GUARDS];			//RE
	ID3D11Texture2D*			m_guatdShaderResourceTex[MAX_GUARDS];	//RE	
	ID3D11RenderTargetView*		m_guardRenderTargetView[MAX_GUARDS];	//RE
	ID3D11ShaderResourceView*	m_guardShaderResource[MAX_GUARDS];		//RE
	ID3D11Buffer*				m_objectBuffer;				//RE

	ID3D11Buffer*				m_textureBuffer;			//RE
	TextureBuffer				m_textureValues;

	Animation::AnimationCBuffer * m_animationBuffer;
public:
	VisabilityPass();
	~VisabilityPass();
	HRESULT Init(ForwardRender* forwardRender) override;
	HRESULT Release() override;

	HRESULT Draw() override;
	HRESULT Draw(Drawable* drawable, const Camera& camera) override;
	HRESULT Draw(std::vector<DX::INSTANCING::GROUP> group, const Camera& camera) override;

	void GuardDepthPrePassFor(VisibilityComponent * target, ForwardRender * forwardRender, Animation::AnimationCBuffer * animBuffer = nullptr);
	void CalculateVisabilityFor(const unsigned short int & index, VisibilityComponent * target, Animation::AnimationCBuffer * animBuffer = nullptr);
	void SetViewportAndRenderTarget(const unsigned short int & index);
private:
	HRESULT _init();
	void _initViewPort();
	HRESULT _initViewBuffer();
	HRESULT _initObjectBuffer();
	HRESULT _initDSV();
	
	HRESULT _initShaders();
	HRESULT _initVertexShaders();
	HRESULT _initPixelShaders();

	HRESULT _mapViewBuffer(const unsigned short int & index, VisibilityComponent * target);

	void _mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer);

	HRESULT _mapObjectBuffer(Drawable * target);

	HRESULT _drawForPlayer(Drawable * player, VisibilityComponent * enemy, int playerIndex);
};