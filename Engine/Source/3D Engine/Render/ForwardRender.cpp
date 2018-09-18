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
	   
	_CreateConstantBuffer();
	_CreateSamplerState();
	shadowMap.Init(128, 128);

	
	
	this->CREATE_VIEWPROJ();
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
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);

	_mapLightInfoNoMatrix();

	shadowMap.SetSamplerAndShaderResources();
	if (!DX::g_lights.empty())
	{
		shadowMap.mapAllLightMatrix(DX::g_lights[0]);

	}
	_mapCameraBufferToVertex(camera);
	_mapCameraBufferToPixel(camera);
	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		switch (DX::g_geometryQueue[i]->getObjectType())
		{
		case Static:
			vertexSize = sizeof(StaticVertex);
			break;
		case Dynamic:
			vertexSize = sizeof(DynamicVertex);
			break;
		}
		
		_SetShaders(i);

		ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_geometryQueue[i]);
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_geometryQueue[i]->BindTextures();
		DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->VertexSize(), 0);



	}

	DX::g_geometryQueue.clear();
	DX::g_lights.clear();
}

void ForwardRender::Flush(Camera & camera)
{
	_SimpleLightCulling(camera);
	this->shadowMap.ShadowPass();
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
	DX::SafeRelease(m_samplerState);

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

void ForwardRender::_SetShaders(int i)
{
	if (m_lastVertexPath != DX::g_geometryQueue[i]->getVertexPath())
	{
		DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DX::g_geometryQueue[i]->getVertexPath()));

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
