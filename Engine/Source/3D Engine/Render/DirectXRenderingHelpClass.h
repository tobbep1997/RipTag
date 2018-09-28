#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")


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
	static HRESULT CreateConstantBuffer(ID3D11Buffer *& buffer, UINT size);
	static HRESULT CreateSamplerState(ID3D11SamplerState *& sampler, 
		D3D11_TEXTURE_ADDRESS_MODE d3_tx_ad_mode = D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_FILTER d3_filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_FUNC d3_comp_func = D3D11_COMPARISON_NEVER,
		float boderColor = 1.0f,
		float minLod = -D3D11_FLOAT32_MAX,
		float maxLod = D3D11_FLOAT32_MAX,
		float mipLoadBias = 0.0f,
		UINT maxAnisotropy = 0);
	static HRESULT CreateTexture2D(ID3D11Texture2D *& texture,
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
	static HRESULT CreateDepthStencilView(ID3D11Resource * resource,
		ID3D11DepthStencilView *& dsv,
		UINT flags = 0,
		DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS,
		D3D11_DSV_DIMENSION dimension = D3D11_DSV_DIMENSION_TEXTURE2D,
		UINT texture2DArray_FirstArraySize = 0,
		UINT texture2DArray_ArraySize = 1,
		UINT texture2DArray_MipSlice = 0);

	static HRESULT CreateShaderResourceView(ID3D11Resource * resource,
		ID3D11ShaderResourceView *& srv,
		UINT flags = 0,
		DXGI_FORMAT format = DXGI_FORMAT_R32_TYPELESS,
		D3D11_SRV_DIMENSION dimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		UINT texture2DArray_ArraySize = 1,
		UINT texture2DArray_FirstArraySlice = 0,
		UINT texture2DArray_MostDetailMip = 0,
		UINT texture2DArray_MipLevel = 1);

	static HRESULT CreateRenderTargetView(ID3D11Resource * resource,
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

};

