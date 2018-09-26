#pragma once
#include "../../Camera.h"
#include "../../../Shader/ShaderManager.h"

class VisabilityPass
{
private:
	const short int GUARD_RES = 32;
	
	const wchar_t * DEPTH_PRE_PASS_VERTEX_SHADER_PATH = L"../Engine/Source/Shader/Shaders/VisabilityShader/PreDepthPassVertex.hlsl";
	const wchar_t * VISABILITY_PASS_PIXEL_SHADER_PATH = L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl";
	const wchar_t * STATIC_VERTEX_SHADER_PATH = L"../Engine/Source/Shader/VertexShader.hlsl";


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
	ID3D11ShaderResourceView *	m_guardShaderResource;

	ID3D11Buffer*				m_objectBuffer;

public:
	VisabilityPass();
	~VisabilityPass();
	void GuardDepthPrePassFor(Guard * guard);
	void CalculateVisabilityFor(Guard * guard);
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

	void _mapViewBuffer(Guard * target);


	void _mapObjectBuffer(Drawable * target);
};