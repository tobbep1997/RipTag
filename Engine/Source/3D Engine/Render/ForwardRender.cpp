#include "ForwardRender.h"
#include "../Extern.h"
#include "../../ShaderCreator.h"
#include "../RenderingManager.h"

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
	D3D11_INPUT_ELEMENT_DESC animatedInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTINFLUENCES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//TODO:: ADD NEW SHADER FOR ANIMATED OBJECTS, CONNECT TOANIMATEDINPUTDESC
	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl", "main", animatedInputDesc, 6);
	//DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");

	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 4);
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");

	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl");

	_CreateConstantBuffer();
	_CreateSamplerState();
	shadowMap.Init(64, 64);

	
	
	this->CREATE_VIEWPROJ();

	this->_createUAV();
}

struct TriangleVertex
{
	float x, y, z, w;
	float r, g, b, a;
};

void ForwardRender::GeometryPass(Camera & camera)
{
	float c[4] = { 0.0f,0.0f,0.5f,1.0f };
	
	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);	
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	//DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);

	DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
		1,
		&m_backBufferRTV,
		m_depthStencilView,
		1, 1, &m_visabilityUAV, 0
	);

	_mapLightInfoNoMatrix();

	shadowMap.SetSamplerAndShaderResources();
	if (!DX::g_lights.empty())
	{
		shadowMap.mapAllLightMatrix(&DX::g_lights);

	}
	_mapCameraBufferToVertex(camera);
	_mapCameraBufferToPixel(camera);
	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	_SetStaticShaders();
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

			_mapObjectBuffer(DX::g_geometryQueue[i]);
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->VertexSize(), 0);
		
	}

	DX::g_geometryQueue.clear();


	DX::g_deviceContext->CopyResource(m_uavTextureBufferCPU, m_uavTextureBuffer);
	D3D11_MAPPED_SUBRESOURCE mr;
	
	struct ShadowTestData
	{
		unsigned int inside;
		unsigned int outside;
	};

	if (SUCCEEDED(DX::g_deviceContext->Map(m_uavTextureBufferCPU, 0, D3D11_MAP_READ, 0, &mr)))
	{
		ShadowTestData* data = (ShadowTestData*)mr.pData;

		data = data;
		
		DX::g_deviceContext->Unmap(m_uavTextureBufferCPU, 0);
	}

}

void ForwardRender::AnimatedGeometryPass(Camera & camera)
{
	float c[4] = { 1.0f,0.0f,1.0f,1.0f };

	//DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	//DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);


	_mapLightInfoNoMatrix();

	shadowMap.SetSamplerAndShaderResources();

	
	_mapCameraBufferToVertex(camera);
	_mapCameraBufferToPixel(camera);
	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;
	_SetAnimatedShaders();
	for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
	{
		

		ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		//DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->VertexSize(), 0);
		DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->VertexSize(), 0);


	}

	DX::g_animatedGeometryQueue.clear();
	DX::g_lights.clear();
}

void ForwardRender::Flush(Camera & camera)
{
	_SimpleLightCulling(camera);
	this->shadowMap.ShadowPass();

	//TODO
	VisabilityPass();

	this->GeometryPass(camera);
	this->AnimatedGeometryPass(camera);
	
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
	DX::SafeRelease(m_samplerState);

	DX::SafeRelease(m_uavTextureBuffer);
	DX::SafeRelease(m_uavTextureBufferCPU);
	DX::SafeRelease(m_visabilityUAV);


	//DX::SafeRelease(m_uavKILLER);

	//DX::SafeRelease(m_visLightBuffer);

	shadowMap.Release();
}

void ForwardRender::_SimpleLightCulling(Camera & cam)
{
	float culled = 0;
#if _DEBUG
	ImGui::Begin("Light Culling");
#endif
	//--------------------------------
	///Early KILL of lights
	//This is the initial culling, it will cullaway the lights that are too far away
	for (size_t i = 0; i < DX::g_lights.size(); i++)
	{
		//Loops every queued light
		if (DX::g_lights.at(i)->GetDistanceFromCamera(cam) >= m_lightCullingDistance)
		{
			DX::g_lights.erase(DX::g_lights.begin() + i);
			culled += 1;
		}
	}
	//-----------------------------------

	//Start Extream light kill
	int needToRemove = DX::g_lights.size() - m_forceCullingLimit;

	if (DX::g_lights.size() > m_forceCullingLimit)
	{
		std::vector<sortStruct> lightRemoval;
		//-----------------------------
		///Calculating the distance of the lights
		//The SortStruct can be removed but not know
		//TODO::remove sortStruct
		for (size_t i = 0; i < DX::g_lights.size(); i++)
		{
			sortStruct temp;
			temp.distance = DX::g_lights.at(i)->GetDistanceFromCamera(cam);
			temp.lightBelong = i;
			lightRemoval.push_back(temp);
		}
		//
		//------------------------------
		///BubbleSorting the values. 1->...->100000
		//
		bool sorted = false;
		while (false == sorted)
		{
			sorted = true;
			for (int i = 0; i < lightRemoval.size() - 1; ++i)
			{
				if (lightRemoval.at(i).distance > lightRemoval.at(i + 1).distance)
				{
					sorted = false;
					sortStruct swap = lightRemoval.at(i);
					lightRemoval.at(i) = lightRemoval.at(i + 1);
					lightRemoval.at(i + 1) = swap;

				}
			}
		}
		//Stop BubbleSort
		//-------------------------------
		///StartPoppin the lights
		for (size_t i = 0; i < needToRemove; i++)
		{
			DX::g_lights.pop_back();
			culled++;
		}
		//--------------------------------
	}
	
	//TODO::
	//Check distance and check if behind then FORCE CULL THAT BITCH

#if _DEBUG
	ImGui::Text("LightsCulled %f", culled);
	ImGui::End();
#endif
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

	//D3D11_BUFFER_DESC pointBufferDesc;
	//lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//lightBufferDesc.ByteWidth = sizeof(PointLightBuffer);
	//lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//lightBufferDesc.MiscFlags = 0;
	//lightBufferDesc.StructureByteStride = 0;

	//hr = DX::g_device->CreateBuffer(&pointBufferDesc, nullptr, &this->m_visLightBuffer);
	//if (FAILED(hr))
	//{
	//	// handle the error, could be fatal or a warning...
	//	exit(-1);
	//}
}


void ForwardRender::_CreateSamplerState()
{
	D3D11_SAMPLER_DESC ssDesc = {};
	ssDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ssDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ssDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ssDesc.MaxAnisotropy = 1;
	ssDesc.MaxLOD = FLT_MAX;
	ssDesc.MinLOD = -FLT_MAX;
	ssDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	HRESULT hr = DX::g_device->CreateSamplerState(&ssDesc, &m_samplerState);
	assert(hr == S_OK);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
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

void ForwardRender::_mapCameraBufferToVertex(Camera & camera)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	m_cameraValues.cameraPosition = camera.getPosition();
	m_cameraValues.viewProjection = camera.getViewProjection();
	DX::g_deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	memcpy(dataPtr.pData, &m_cameraValues, sizeof(CameraBuffer));
	// UnMap constant buffer so that we can use it again in the GPU
	DX::g_deviceContext->Unmap(m_cameraBuffer, 0);
	// set resource to Vertex Shader
	DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_cameraBuffer);
}
void ForwardRender::_mapCameraBufferToPixel(Camera & camera)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	m_cameraValues.cameraPosition = camera.getPosition();
	m_cameraValues.viewProjection = camera.getViewProjection();
	DX::g_deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	memcpy(dataPtr.pData, &m_cameraValues, sizeof(CameraBuffer));
	// UnMap constant buffer so that we can use it again in the GPU
	DX::g_deviceContext->Unmap(m_cameraBuffer, 0);
	// set resource to Vertex Shader
	DX::g_deviceContext->PSSetConstantBuffers(2, 1, &m_cameraBuffer);
}
void ForwardRender::_mapLightInfoNoMatrix()
{
	m_lightValues.info = DirectX::XMINT4((int32_t)DX::g_lights.size(), 0, 0, 0);
	for (unsigned int i = 0; i < DX::g_lights.size(); i++)
	{
		m_lightValues.position[i] = DX::g_lights[i]->getPosition();
		m_lightValues.color[i] = DX::g_lights[i]->getColor();
		m_lightValues.dropOff[i] = DirectX::XMFLOAT4A(DX::g_lights[i]->getDropOff(), 0,0,0);
	}
	for (unsigned int i = (unsigned int)DX::g_lights.size(); i < 8; i++)
	{
		m_lightValues.position[i] = DirectX::XMFLOAT4A(0,0,0,0);
		m_lightValues.color[i] = DirectX::XMFLOAT4A(0,0,0,0);
		m_lightValues.dropOff[i] = DirectX::XMFLOAT4A(0, 0, 0, 0);
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


void ForwardRender::_SetStaticShaders()
{
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DX::g_geometryQueue[0]->getVertexPath()));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DX::g_geometryQueue[0]->getVertexPath()), nullptr, 0);
	

	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	if (m_lastPixelPath != DX::g_geometryQueue[0]->getPixelPath())
	{
		DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(DX::g_geometryQueue[0]->getPixelPath()), nullptr, 0);
	}
}

void ForwardRender::VisabilityPass()
{
	float c[4] = { 0.0f,0.0f,0.5f,1.0f };

	//DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	//DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);

	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl"), nullptr, 0);

	//DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);

	DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
		1,
		&m_backBufferRTV,
		//m_depthStencilView,
		nullptr,
		1, 1, &m_visabilityUAV, 0
	);

	for (unsigned int i = 0; i < DX::g_visabilityDrawQueue.size(); ++i)
	{
		UINT32 vertexSize = sizeof(StaticVertex);
		UINT32 offset = 0;

		ID3D11Buffer * vertexBuffer = DX::g_visabilityDrawQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_visabilityDrawQueue[i]);
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_visabilityDrawQueue[i]->VertexSize(), 0);
	}

	DX::g_visabilityDrawQueue.clear();


	DX::g_deviceContext->CopyResource(m_uavTextureBufferCPU, m_uavTextureBuffer);
	D3D11_MAPPED_SUBRESOURCE mr;

	struct ShadowTestData
	{
		unsigned int inside;
		unsigned int outside;
	};

	if (SUCCEEDED(DX::g_deviceContext->Map(m_uavTextureBufferCPU, 0, D3D11_MAP_READ, 0, &mr)))
	{
		ShadowTestData* data = (ShadowTestData*)mr.pData;

		//std::cout << data->inside <<std::endl;
		DX::g_deviceContext->Unmap(m_uavTextureBufferCPU, 0);
	}
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	if (SUCCEEDED(DX::g_deviceContext->Map(m_uavTextureBufferCPU, 0, D3D11_MAP_WRITE, 0, &dataPtr)))
	{
		ShadowTestData hiv ={ 0,0 };
		memcpy(dataPtr.pData, &hiv, sizeof(ShadowTestData));
		DX::g_deviceContext->CopyResource(m_uavTextureBuffer, m_uavTextureBufferCPU);
		//DX::g_deviceContext->CopyResource(m_uavTextureBuffer, m_uavTextureBufferCPU);
		DX::g_deviceContext->Unmap(m_uavTextureBufferCPU, 0);
	}
	
}

void ForwardRender::_SetAnimatedShaders()
{
	if (DX::g_animatedGeometryQueue.size() > 0)
	{
		DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DX::g_animatedGeometryQueue[0]->getVertexPath()));
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DX::g_animatedGeometryQueue[0]->getVertexPath()), nullptr, 0);
		DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
		DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
		DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
		if (m_lastPixelPath != DX::g_animatedGeometryQueue[0]->getPixelPath())
		{
			DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(DX::g_animatedGeometryQueue[0]->getPixelPath()), nullptr, 0);
		}
	}
	
}

void ForwardRender::_createUAV()
{
	D3D11_TEXTURE2D_DESC TextureData;
	ZeroMemory(&TextureData, sizeof(TextureData));
	TextureData.ArraySize = 1;
	TextureData.Height = 1;
	TextureData.Width = 2;
	TextureData.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	TextureData.CPUAccessFlags = 0;
	TextureData.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	TextureData.MipLevels = 1;
	TextureData.MiscFlags = 0;
	TextureData.SampleDesc.Count = 1;
	TextureData.SampleDesc.Quality = 0;
	TextureData.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr;
	//D3D11_SUBRESOURCE_DATA InitialData;
	//ZeroMemory(&InitialData, sizeof(InitialData));
	////InitialData.pSysMem = pData;
	//InitialData.SysMemPitch = 1 * sizeof(UINT);
	//InitialData.SysMemSlicePitch = 1 * sizeof(UINT) * 1;
	hr = DX::g_device->CreateTexture2D(&TextureData, NULL, &m_uavTextureBuffer);

	TextureData.Usage = D3D11_USAGE_STAGING;
	TextureData.BindFlags = 0;
	TextureData.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	hr = DX::g_device->CreateTexture2D(&TextureData, 0, &m_uavTextureBufferCPU);

	//TextureData.Usage = D3D11_USAGE_STAGING;
	//TextureData.BindFlags = 0;
	//TextureData.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//hr = DX::g_device->CreateTexture2D(&TextureData, 0, &m_uavKILLER);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	ZeroMemory(&UAVdesc, sizeof(UAVdesc));
	UAVdesc.Format = DXGI_FORMAT_R32_UINT;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Texture2D.MipSlice = 0;
	hr = DX::g_device->CreateUnorderedAccessView(m_uavTextureBuffer, &UAVdesc, &m_visabilityUAV);

}