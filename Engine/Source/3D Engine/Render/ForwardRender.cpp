#include "ForwardRender.h"
#include "../Extern.h"
#include "../../ShaderCreator.h"

ForwardRender::ForwardRender()
{
	m_lastVertexPath = L"NULL";
	m_lastPixelPath = L"NULL";
}


ForwardRender::~ForwardRender()
{
}

void ForwardRender::Init(	IDXGISwapChain*				swapChain,
							ID3D11RenderTargetView*		backBufferRTV,
							ID3D11DepthStencilView*		depthStencilView,
							ID3D11Texture2D*			depthBufferTex,
							ID3D11SamplerState*			samplerState,
							D3D11_VIEWPORT				viewport)
{
	m_swapChain = swapChain;
	m_backBufferRTV = backBufferRTV;
	m_depthStencilView = depthStencilView;
	m_depthBufferTex = depthBufferTex;
	m_samplerState = samplerState;
	m_viewport = viewport;
	
	float c[4] = { 1.0f,0.0f,1.0f,1.0f };

	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 4);
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");


	D3D11_INPUT_ELEMENT_DESC shadowInputDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/Shaders/ShadowVertex.hlsl", "main", inputDesc, 4);

	_createShadowViewPort(128, 128);
	_createShadowDepthStencilView(128, 128);
	_CreateConstantBuffer();
	
	this->CREATE_VIEWPROJ();
}

struct TriangleVertex
{
	float x, y, z, w;
	float r, g, b, a;
};

void ForwardRender::ShadowPass()
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

void ForwardRender::GeometryPass(Camera & camera)
{
	float c[4] = { 1.0f,0.0f,1.0f,1.0f };
	
	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);	
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	

	_mapLightInfoNoMatrix();

	DX::g_deviceContext->PSSetSamplers(0, 1, &m_shadowSamplerState);
	for (int i = 0; i < 6; i++)
	{
		DX::g_deviceContext->PSSetShaderResources(i, 1, &m_shadowShaderResourceView[i]);
	}

	_mapAllLightMatrix(DX::g_lights[0]);
	_mapCameraBuffer(camera);
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		UINT32 vertexSize = sizeof(StaticVertex);
		UINT32 offset = 0;
		
		_SetShaders(i);

		ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_geometryQueue[i]);
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->VertexSize(), 0);



	}

	DX::g_geometryQueue.clear();
	DX::g_lights.clear();
}

void ForwardRender::Flush(Camera & camera)
{
	this->ShadowPass();
	this->GeometryPass(camera);
}

void ForwardRender::Present()
{
	m_swapChain->Present(0, 0);
}

void ForwardRender::Release()
{
	DX::SafeRelease(m_objectBuffer);
	DX::SafeRelease(m_cameraBuffer);
	DX::SafeRelease(m_lightBuffer);
	for (int i = 0; i < 6; i++)
	{
		DX::SafeRelease(m_shadowDepthStencilView[i]);
		DX::SafeRelease(m_shadowDepthBufferTex[i]);
		DX::SafeRelease(m_shadowShaderResourceView[i]);
	}
	DX::SafeRelease(m_shadowSamplerState);
	DX::SafeRelease(m_lightMatrixBuffer);
	DX::SafeRelease(m_allLightMatrixBuffer);
}


void ForwardRender::_CreateConstantBuffer()
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

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBuffer);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	hr = DX::g_device->CreateBuffer(&cameraBufferDesc, nullptr, &this->m_cameraBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	hr = DX::g_device->CreateBuffer(&lightBufferDesc, nullptr, &this->m_lightBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}

	D3D11_BUFFER_DESC lightMatrixBufferDesc;
	lightMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightMatrixBufferDesc.ByteWidth = sizeof(CameraBuffer);
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

void ForwardRender::_mapObjectBuffer(Drawable * drawable)
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

void ForwardRender::_mapCameraBuffer(Camera & camera)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	m_cameraValues.viewProjection = camera.getViewProjection();
	DX::g_deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	memcpy(dataPtr.pData, &m_cameraValues, sizeof(CameraBuffer));
	// UnMap constant buffer so that we can use it again in the GPU
	DX::g_deviceContext->Unmap(m_cameraBuffer, 0);
	// set resource to Vertex Shader
	DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_cameraBuffer);
}
void ForwardRender::_mapLightInfoNoMatrix()
{
	m_lightValues.info = DirectX::XMINT4((int32_t)DX::g_lights.size(), 0, 0, 0);
	for (unsigned int i = 0; i < DX::g_lights.size(); i++)
	{
		m_lightValues.position[i] = DX::g_lights[i]->getPosition();
		m_lightValues.color[i] = DX::g_lights[i]->getColor();
		m_lightValues.dropOff[i] = DX::g_lights[i]->getDropOff();
	}
	for (size_t i = DX::g_lights.size(); i < 8; i++)
	{
		m_lightValues.position[i] = DirectX::XMFLOAT4A();
		m_lightValues.color[i] = DirectX::XMFLOAT4A();
		m_lightValues.dropOff[i] = 0.0f;
	}

	D3D11_MAPPED_SUBRESOURCE dataPtr;


	DX::g_deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &m_lightValues, sizeof(LightBuffer));
	DX::g_deviceContext->Unmap(m_lightBuffer, 0);
	DX::g_deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);
}
void ForwardRender::CREATE_VIEWPROJ()
{
	using namespace DirectX;

	XMFLOAT3 cameraStored = XMFLOAT3(0, 0, -5);

	XMFLOAT3 lookAtStored = XMFLOAT3(0, 0, 1);

	XMFLOAT3 UP_STORED = XMFLOAT3(0, 1, 0);

	XMVECTOR cameraPos = XMLoadFloat3(&cameraStored);

	XMVECTOR lookAt = XMLoadFloat3(&lookAtStored);

	XMVECTOR UP = XMLoadFloat3(&UP_STORED);
	XMStoreFloat4x4A(&this->projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI * 0.5f, 1/1, 0.1f, 20)));
	XMStoreFloat4x4A(&this->view,XMMatrixTranspose(XMMatrixLookToLH(cameraPos, lookAt, UP)));

}

void ForwardRender::_mapLightMatrix(PointLight * light, unsigned int i)
{
	m_lightMatrixValues.viewProjection = light->getSides()[i]->getViewProjection();


	D3D11_MAPPED_SUBRESOURCE dataPtr;
	DX::g_deviceContext->Map(m_lightMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &m_lightMatrixValues, sizeof(CameraBuffer));
	DX::g_deviceContext->Unmap(m_lightMatrixBuffer, 0);
	DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_lightMatrixBuffer);
	//DX::g_deviceContext->PSSetConstantBuffers(1, 1, &m_lightMatrixBuffer);
	
}

void ForwardRender::_mapAllLightMatrix(PointLight * light)
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

void ForwardRender::_createShadowViewPort(UINT sizeX, UINT sizeY)
{
	m_shadowViewport.Width = static_cast<float>(sizeX);
	m_shadowViewport.Height = static_cast<float>(sizeY);
	m_shadowViewport.MinDepth = 0.0f;
	m_shadowViewport.MaxDepth = 1.0f;
	m_shadowViewport.TopLeftX = 0;
	m_shadowViewport.TopLeftY = 0;
}

void ForwardRender::_createShadowDepthStencilView(UINT width, UINT hight)
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
	samplerDescPoint.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescPoint.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescPoint.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescPoint.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDescPoint.MipLODBias = 0.0f;
	samplerDescPoint.MaxAnisotropy = 1;
	samplerDescPoint.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDescPoint.MinLOD = -FLT_MAX;
	samplerDescPoint.MaxLOD = FLT_MAX;

	

	hr = DX::g_device->CreateSamplerState(&samplerDescPoint, &m_shadowSamplerState);

}




void ForwardRender::_SetShaders(int i)
{
	if (m_lastVertexPath != DX::g_geometryQueue[i]->getVertexPath())
	{
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DX::g_geometryQueue[i]->getVertexPath()), nullptr, 0);
	}
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	if (m_lastPixelPath != DX::g_geometryQueue[i]->getPixelPath())
	{
		DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(DX::g_geometryQueue[i]->getPixelPath()), nullptr, 0);
	}
}
