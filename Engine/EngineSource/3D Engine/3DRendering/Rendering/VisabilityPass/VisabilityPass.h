#pragma once
#include "../../../Components/Camera.h"
#include "../../../../Shader/ShaderManager.h"
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


	struct GuardViewBuffer
	{
		DirectX::XMFLOAT4A cameraPosition;
		DirectX::XMFLOAT4X4A viewProjection;
	};
	struct ObjectBuffer
	{
		DirectX::XMFLOAT4X4A worldMatrix;
	};

private:
	D3D11_VIEWPORT				m_guardViewPort;
	ID3D11Buffer *				m_guardViewBuffer;
	ID3D11DepthStencilView*		m_guardDepthStencil;
	ID3D11Texture2D*			m_guardDepthTex;
	ID3D11Texture2D*			m_guatdShaderResourceTex;
	ID3D11RenderTargetView*		m_guardRenderTargetView;
	ID3D11ShaderResourceView*	m_guardShaderResource;
	ID3D11Buffer*				m_objectBuffer;

public:
	VisabilityPass();
	~VisabilityPass();
	void Init();
	void GuardDepthPrePassFor(VisibilityComponent * target, Animation::AnimationCBuffer * animBuffer = nullptr);
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
};