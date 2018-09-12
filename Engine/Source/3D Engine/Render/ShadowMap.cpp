#include "ShadowMap.h"


ShadowMap::ShadowMap()
{
}


ShadowMap::~ShadowMap()
{
}

void ShadowMap::Init(UINT width, UINT height)
{
	_createBuffers();
	_createShadowViewPort(width, height);
	_createShadowDepthStencilView(width, height);
}



void ShadowMap::ShadowPass()
{
	for (int i = 0; i < 6; i++)
	{
		DX::g_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/ShadowVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_shadowViewport);

	for (int x = 0; x < DX::g_lights.size(); x++)
	{

		for (int i = 0; i < 6; i++)
		{
			DX::g_deviceContext->OMSetRenderTargets(0, nullptr, m_shadowDepthStencilView[i]);

			_mapLightMatrix(DX::g_lights[x], i);
			for (unsigned int j = 0; j < DX::g_geometryQueue.size(); j++)
			{
				UINT32 vertexSize = sizeof(StaticVertex);
				UINT32 offset = 0;

				ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[j]->getBuffer();

				_mapObjectBuffer(DX::g_geometryQueue[j]);
				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
				DX::g_deviceContext->Draw(DX::g_geometryQueue[j]->VertexSize(), 0);
			}
		}
	}
}

void ShadowMap::mapAllLightMatrix(PointLight * light)
{

	for (int i = 0; i < 6; i++)
	{
		m_allLightMatrixValues.viewProjection[i] = light->getSides()[i]->getViewProjection();
	}


	D3D11_MAPPED_SUBRESOURCE dataPtr;
	DX::g_deviceContext->Map(m_allLightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &m_allLightMatrixValues, sizeof(PointLightBuffer));
	DX::g_deviceContext->Unmap(m_allLightMatrixBuffer, 0);
	//DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_lightMatrixBuffer);
	DX::g_deviceContext->PSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);

}

void ShadowMap::SetSamplerAndShaderResources()
{
	DX::g_deviceContext->PSSetSamplers(0, 1, &m_shadowSamplerState);
	for (int i = 0; i < 6; i++)
	{
		DX::g_deviceContext->PSSetShaderResources(i, 1, &m_shadowShaderResourceView[i]);
	}
}

void ShadowMap::Release()
{
	DX::SafeRelease(m_shadowSamplerState);
	for (int i = 0; i < 6; i++)
	{
		DX::SafeRelease(m_shadowDepthStencilView[i]);
		DX::SafeRelease(m_shadowDepthBufferTex[i]);
		DX::SafeRelease(m_shadowShaderResourceView[i]);
	}
	DX::SafeRelease(m_objectBuffer);
	DX::SafeRelease(m_lightMatrixBuffer);
	DX::SafeRelease(m_allLightMatrixBuffer);
}

void ShadowMap::_createShadowViewPort(UINT width, UINT height)
{
	m_shadowViewport.Width = static_cast<float>(width);
	m_shadowViewport.Height = static_cast<float>(height);
	m_shadowViewport.MinDepth = 0.0f;
	m_shadowViewport.MaxDepth = 1.0f;
	m_shadowViewport.TopLeftX = 0;
	m_shadowViewport.TopLeftY = 0;
}

void ShadowMap::_createShadowDepthStencilView(UINT width, UINT hight)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = hight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = depthStencilDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	HRESULT hr;
	for (int i = 0; i < 6; i++)
	{
		hr = DX::g_device->CreateTexture2D(&depthStencilDesc, NULL, &m_shadowDepthBufferTex[i]);
		hr = DX::g_device->CreateDepthStencilView(m_shadowDepthBufferTex[i], &dsvDesc, &m_shadowDepthStencilView[i]);
		hr = DX::g_device->CreateShaderResourceView(m_shadowDepthBufferTex[i], &srvDesc, &m_shadowShaderResourceView[i]);
	}

	//Create the Depth/Stencil View
	D3D11_SAMPLER_DESC samplerDescPoint;
	samplerDescPoint.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescPoint.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescPoint.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDescPoint.BorderColor[0] = 1.0f;
	samplerDescPoint.BorderColor[1] = 1.0f;
	samplerDescPoint.BorderColor[2] = 1.0f;
	samplerDescPoint.BorderColor[3] = 1.0f;
	samplerDescPoint.MinLOD = 0.f;
	samplerDescPoint.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDescPoint.MipLODBias = 0.f;
	samplerDescPoint.MaxAnisotropy = 0;
	samplerDescPoint.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	samplerDescPoint.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

	hr = DX::g_device->CreateSamplerState(&samplerDescPoint, &m_shadowSamplerState);
}

void ShadowMap::_createBuffers()
{
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(ObjectBuffer);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = 0;
	hr = DX::g_device->CreateBuffer(&objectBufferDesc, nullptr, &m_objectBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}

	D3D11_BUFFER_DESC lightMatrixBufferDesc;
	lightMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightMatrixBufferDesc.ByteWidth = sizeof(LightCameraBuffer);
	lightMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightMatrixBufferDesc.MiscFlags = 0;
	lightMatrixBufferDesc.StructureByteStride = 0;

	hr = DX::g_device->CreateBuffer(&lightMatrixBufferDesc, nullptr, &this->m_lightMatrixBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}

	D3D11_BUFFER_DESC allLightMatrixBufferDesc;
	allLightMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	allLightMatrixBufferDesc.ByteWidth = sizeof(PointLightBuffer);
	allLightMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	allLightMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	allLightMatrixBufferDesc.MiscFlags = 0;
	allLightMatrixBufferDesc.StructureByteStride = 0;

	hr = DX::g_device->CreateBuffer(&allLightMatrixBufferDesc, nullptr, &this->m_allLightMatrixBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}
}

void ShadowMap::_mapLightMatrix(PointLight * pointLight, unsigned int i)
{
	m_lightMatrixValues.viewProjection = pointLight->getSides()[i]->getViewProjection();


	D3D11_MAPPED_SUBRESOURCE dataPtr;
	DX::g_deviceContext->Map(m_lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &m_lightMatrixValues, sizeof(LightCameraBuffer));
	DX::g_deviceContext->Unmap(m_lightMatrixBuffer, 0);
	DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_lightMatrixBuffer);
}



void ShadowMap::_mapObjectBuffer(Drawable * drawable)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;

	m_objectValues.worldMatrix = drawable->getWorldmatrix();
	DX::g_deviceContext->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	// copy memory from CPU to GPU the entire struct
	memcpy(dataPtr.pData, &m_objectValues, sizeof(ObjectBuffer));
	// UnMap constant buffer so that we can use it again in the GPU
	DX::g_deviceContext->Unmap(m_objectBuffer, 0);
	// set resource to Vertex Shader
	DX::g_deviceContext->VSSetConstantBuffers(0, 1, &m_objectBuffer);
}
