#include "EnginePCH.h"
#include "DirectXRenderingHelpClass.h"

#ifndef _DEPLOY
#include <D3DCommon.h>
#pragma comment( lib, "dxguid.lib") 
#endif

HRESULT DXRHC::CreateConstantBuffer(const std::string & name, ID3D11Buffer *& buffer, UINT size)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr;// = DX::g_device->CreateBuffer(&bufferDesc, nullptr, &buffer);
	if (SUCCEEDED(hr = DX::g_device->CreateBuffer(&bufferDesc, nullptr, &buffer)))
	{
		DX::SetName(buffer, name);
	}
	else
		exit(-1);

}

HRESULT DXRHC::CreateSamplerState(const std::string & name, ID3D11SamplerState *& sampler, D3D11_TEXTURE_ADDRESS_MODE d3_tx_ad_mode, D3D11_FILTER d3_filter, D3D11_COMPARISON_FUNC d3_comp_func, float boderColor, float minLod, float maxLod, float mipLoadBias, UINT maxAnisotropy)
{
	D3D11_SAMPLER_DESC ssDesc = {};
	ssDesc.AddressU = d3_tx_ad_mode;
	ssDesc.AddressV = d3_tx_ad_mode;
	ssDesc.AddressW = d3_tx_ad_mode;
	ssDesc.ComparisonFunc = d3_comp_func;
	ssDesc.MaxAnisotropy = maxAnisotropy;
	ssDesc.Filter = d3_filter;
	ssDesc.MaxLOD = maxLod;
	ssDesc.MinLOD = minLod;
	for (unsigned int i = 0; i < 4; i++)
		ssDesc.BorderColor[i] = boderColor;


	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateSamplerState(&ssDesc, &sampler)))
	{
		DX::SetName(sampler, name);
	}
	else
		exit(-1);

	return hr;
}

HRESULT DXRHC::CreateTexture2D(const std::string & name, ID3D11Texture2D *& texture, UINT height, UINT width, UINT bindFlags, UINT mipLevel, UINT sampleDescCount, UINT sampleDescQuality, UINT arraySize, UINT CPUAccessFlags, UINT miscFlags, DXGI_FORMAT format, D3D11_USAGE usage)
{
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = mipLevel;
	textureDesc.ArraySize = arraySize;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = sampleDescCount;
	textureDesc.SampleDesc.Quality = sampleDescQuality;
	textureDesc.Usage = usage;
	textureDesc.BindFlags = bindFlags;
	textureDesc.CPUAccessFlags = CPUAccessFlags;
	textureDesc.MiscFlags = miscFlags;

	
	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateTexture2D(&textureDesc, NULL, &texture)))
	{
		DX::SetName(texture, name);
	}
	else
		exit(-1);
	return hr;
}

HRESULT DXRHC::CreateDepthStencilView(const std::string & name, ID3D11Resource * resource, ID3D11DepthStencilView *& dsv, UINT flags, DXGI_FORMAT format, D3D11_DSV_DIMENSION dimension, UINT texture2DArray_FirstArraySize, UINT texture2DArray_ArraySize, UINT texture2DArray_MipSlice)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc{};

	depthStencilDesc.Flags = 0;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;

	depthStencilDesc.Texture2DArray.FirstArraySlice = texture2DArray_FirstArraySize;
	depthStencilDesc.Texture2DArray.ArraySize = texture2DArray_ArraySize;
	depthStencilDesc.Texture2DArray.MipSlice = texture2DArray_MipSlice;

	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilView(resource, &depthStencilDesc, &dsv)))
	{
		DX::SetName(dsv, name);
	}
	return hr;
	
}

HRESULT DXRHC::CreateShaderResourceView(const std::string & name, ID3D11Resource * resource, ID3D11ShaderResourceView *& srv, UINT flags, DXGI_FORMAT format, D3D11_SRV_DIMENSION dimension, UINT texture2DArray_ArraySize, UINT texture2DArray_FirstArraySlice, UINT texture2DArray_MostDetailMip, UINT texture2DArray_MipLevel)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.ViewDimension = dimension;
	srvDesc.Texture2DArray.MipLevels = texture2DArray_MipLevel;
	srvDesc.Texture2DArray.MostDetailedMip = texture2DArray_MostDetailMip;
	srvDesc.Texture2DArray.ArraySize = texture2DArray_ArraySize;
	srvDesc.Texture2DArray.FirstArraySlice = texture2DArray_FirstArraySlice;

	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateShaderResourceView(resource, &srvDesc, &srv)))
	{
		DX::SetName(srv, name);
	}
	else
		exit(-1);
	return hr;
}

HRESULT DXRHC::CreateRenderTargetView(const std::string & name, ID3D11Resource * resource, ID3D11RenderTargetView *& rtv, DXGI_FORMAT format, D3D11_RTV_DIMENSION dimension, UINT texture2DArray_ArraySize, UINT texture2DArray_FirstArraySlice, UINT texture2DArray_MipSlice)
{
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};

	
	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = dimension;
	renderTargetViewDesc.Texture2DArray.ArraySize = texture2DArray_ArraySize;
	renderTargetViewDesc.Texture2DArray.FirstArraySlice = texture2DArray_FirstArraySlice;
	renderTargetViewDesc.Texture2DArray.MipSlice = texture2DArray_MipSlice;
	
	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateRenderTargetView(resource, &renderTargetViewDesc, &rtv)))
	{
		DX::SetName(rtv, name);
	}
	else
		exit(-1);
	return hr;
}

void DXRHC::MapBuffer(ID3D11Buffer *& buffer, void* input, unsigned int inputSize, unsigned int slot, unsigned int numBuffer, ShaderTypes i_shader)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr)))
	{
		memcpy(dataPtr.pData, input, inputSize);
		
		DX::g_deviceContext->Unmap(buffer, 0);

		if (numBuffer != 0)
		{
			switch (i_shader)
			{
			case vertex:
				DX::g_deviceContext->VSSetConstantBuffers(slot, numBuffer, &buffer);
				break;
			case hull:
				DX::g_deviceContext->HSSetConstantBuffers(slot, numBuffer, &buffer);
				break;
			case domain:
				DX::g_deviceContext->DSSetConstantBuffers(slot, numBuffer, &buffer);
				break;
			case geometry:
				DX::g_deviceContext->GSSetConstantBuffers(slot, numBuffer, &buffer);
				break;
			case pixel:
				DX::g_deviceContext->PSSetConstantBuffers(slot, numBuffer, &buffer);
				break;
			case compute:
				DX::g_deviceContext->CSSetConstantBuffers(slot, numBuffer, &buffer);
				break;
			}
		}
	}
}

HRESULT DXRHC::CreateRasterizerState(const std::string & name, ID3D11RasterizerState *& rasterrizerState,
	BOOL antialiasedLineEnable ,
	D3D11_CULL_MODE cullMode ,
	INT depthBias,
	FLOAT depthBiasClamp,
	BOOL depthClipEnable,
	D3D11_FILL_MODE fillMode,
	BOOL frontCounterClockwise,
	BOOL multisampleEnable,
	BOOL scissorEnable,
	FLOAT SlopeScaledDepthBias)
{
	D3D11_RASTERIZER_DESC wfdesc{};
	wfdesc.AntialiasedLineEnable = antialiasedLineEnable;
	wfdesc.CullMode = cullMode;
	wfdesc.DepthBias = depthBias;
	wfdesc.DepthBiasClamp = depthBiasClamp;
	wfdesc.DepthClipEnable = depthClipEnable;
	wfdesc.FillMode = fillMode;
	wfdesc.FrontCounterClockwise = frontCounterClockwise;
	wfdesc.MultisampleEnable = multisampleEnable;
	wfdesc.ScissorEnable = FALSE;
	wfdesc.SlopeScaledDepthBias = SlopeScaledDepthBias;

	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateRasterizerState(&wfdesc, &rasterrizerState)))
	{
		DX::SetName(rasterrizerState, name);
	}
	else
		exit(-1);

}

HRESULT DXRHC::CreateBlendState(const std::string & name, ID3D11BlendState *& blendState, BOOL BlendEnable, D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask)
{
	D3D11_BLEND_DESC omDesc{};

	ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = BlendEnable;
	omDesc.RenderTarget[0].SrcBlend = SrcBlend;
	omDesc.RenderTarget[0].DestBlend = DestBlend;
	omDesc.RenderTarget[0].BlendOp = BlendOp;
	omDesc.RenderTarget[0].SrcBlendAlpha = SrcBlendAlpha;
	omDesc.RenderTarget[0].DestBlendAlpha = DestBlendAlpha;
	omDesc.RenderTarget[0].BlendOpAlpha = BlendOpAlpha;
	omDesc.RenderTarget[0].RenderTargetWriteMask = RenderTargetWriteMask;

	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateBlendState(&omDesc, &blendState)))
	{
		DX::SetName(blendState, name);

	}
	else
		exit(-1);
}

