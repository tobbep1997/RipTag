#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>


enum ShaderTypes
{
	vertex,
	hull,
	domain,
	geometry,
	pixel,
	compute
};
//DirectX Rendering Help Class
class DXRHC
{
public:
public:
	static HRESULT CreateConstantBuffer(const std::string & name, ID3D11Buffer *& buffer, UINT size);
	static HRESULT CreateSamplerState(const std::string & name, ID3D11SamplerState *& sampler,
		D3D11_TEXTURE_ADDRESS_MODE d3_tx_ad_mode = D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_FILTER d3_filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_FUNC d3_comp_func = D3D11_COMPARISON_NEVER,
		float boderColor = 1.0f,
		float minLod = -D3D11_FLOAT32_MAX,
		float maxLod = D3D11_FLOAT32_MAX,
		float mipLoadBias = 0.0f,
		UINT maxAnisotropy = 0);
	static HRESULT CreateTexture2D(const std::string & name, ID3D11Texture2D *& texture,
		UINT height,
		UINT width,
		UINT bindFlags = 0,
		UINT mipLevel = 1,
		UINT sampleDescCount = 1,
		UINT sampleDescQuality = 0,
		UINT arraySize = 1,
		UINT CPUAccessFlags = 0,
		UINT miscFlags = 0,
		DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS,
		D3D11_USAGE usage = D3D11_USAGE_DEFAULT
		);
	static HRESULT CreateDepthStencilView(const std::string & name, ID3D11Resource * resource,
		ID3D11DepthStencilView *& dsv,
		UINT flags = 0,
		DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS,
		D3D11_DSV_DIMENSION dimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		UINT texture2DArray_FirstArraySize = 0,
		UINT texture2DArray_ArraySize = 1,
		UINT texture2DArray_MipSlice = 0);

	static HRESULT CreateShaderResourceView(const std::string & name, ID3D11Resource * resource,
		ID3D11ShaderResourceView *& srv,
		UINT flags = 0,
		DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS,
		D3D11_SRV_DIMENSION dimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		UINT texture2DArray_ArraySize = 1,
		UINT texture2DArray_FirstArraySlice = 0,
		UINT texture2DArray_MostDetailMip = 0,
		UINT texture2DArray_MipLevel = 1);

	static HRESULT CreateRenderTargetView(const std::string & name, ID3D11Resource * resource,
		ID3D11RenderTargetView *& rtv,	
		DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS,
		D3D11_RTV_DIMENSION dimension = D3D11_RTV_DIMENSION_TEXTURE2D,
		UINT texture2DArray_ArraySize = 1,
		UINT texture2DArray_FirstArraySlice = 0,
		UINT texture2DArray_MipSlice = 0);

	static void MapBuffer(ID3D11Buffer *& buffer,
		void* input,
		unsigned int inputSize,
		unsigned int slot = 0, 
		unsigned int numBuffer = 0,
		ShaderTypes i_shader = ShaderTypes::vertex);
	static HRESULT CreateRasterizerState(const std::string & name, ID3D11RasterizerState *& rasterrizerState,
		BOOL antialiasedLineEnable = FALSE,
		D3D11_CULL_MODE cullMode = D3D11_CULL_NONE,
		INT depthBias = 0,
		FLOAT depthBiasClamp = 0.0f,
		BOOL depthClipEnable = FALSE,
		D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID,
		BOOL frontCounterClockwise = FALSE,
		BOOL multisampleEnable = FALSE,
		BOOL scissorEnable = FALSE,
		FLOAT SlopeScaledDepthBias = 0.0f);
	static HRESULT CreateBlendState(const std::string & name, ID3D11BlendState *& blendState,
		BOOL BlendEnable = true,
		D3D11_BLEND SrcBlend = D3D11_BLEND_SRC_ALPHA,
		D3D11_BLEND DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_OP BlendOp = D3D11_BLEND_OP_ADD,
		D3D11_BLEND SrcBlendAlpha = D3D11_BLEND_ONE,
		D3D11_BLEND DestBlendAlpha = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP BlendOpAlpha = D3D11_BLEND_OP_ADD,
		UINT8 RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL);

};

