#include "ForwardRender.h"
#include "../Engine/Source/3D Engine/Extern.h"
#include "../Engine/Source/Shader/ShaderManager.h"
#include "../../RenderingManager.h"
#include "../Framework/DirectXRenderingHelpClass.h"
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

	//DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	//DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	_createShaders();
	
	_createConstantBuffer();
	_createSamplerState();
	m_shadowMap.Init(128, 128);


	this->_createUAV();
}

void ForwardRender::GeometryPass(Camera & camera)
{
	
	if (m_firstRun == true)
	{
		m_shaderThreads[0].join();
		m_shaderThreads[1].join();
		m_shaderThreads[2].join();
		m_firstRun = false;
	}
	//DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	/*DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
		1,
		&m_backBufferRTV,
		m_depthStencilView,
		1, 1, &m_visabilityUAV, 0
	);*/

	_mapLightInfoNoMatrix();

	m_shadowMap.SetSamplerAndShaderResources();
	if (!DX::g_lights.empty())
	{
		m_shadowMap.MapAllLightMatrix(&DX::g_lights);

	}
	_mapCameraBufferToVertex(camera);
	_mapCameraBufferToPixel(camera);
	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	_setStaticShaders();
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();
			
			_mapObjectBuffer(DX::g_geometryQueue[i]);
			DX::g_geometryQueue[i]->BindTextures();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
		
	}



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
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);


	_mapLightInfoNoMatrix();

	m_shadowMap.SetSamplerAndShaderResources();

	
	_mapCameraBufferToVertex(camera);
	_mapCameraBufferToPixel(camera);
	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;
	_setAnimatedShaders();
	for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
	{
		

		ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);
		DX::g_animatedGeometryQueue[i]->BindTextures();

		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		//DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->VertexSize(), 0);
		DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);


	}

	//DX::g_animatedGeometryQueue.clear();
	//DX::g_lights.clear();
}

void ForwardRender::Flush(Camera & camera)
{
	_simpleLightCulling(camera);
	this->m_shadowMap.ShadowPass();
	VisabilityPass();
	this->GeometryPass(camera);
	this->AnimatedGeometryPass(camera);
	
}

void ForwardRender::Clear()
{
	float c[4] = { 0.0f,0.0f,0.5f,1.0f };


	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_animatedGeometryQueue.clear();
	DX::g_lights.clear();

	DX::g_geometryQueue.clear();

	DX::g_visabilityDrawQueue.clear();

	this->m_shadowMap.Clear();

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

	m_shadowMap.Release();
}

void ForwardRender::_simpleLightCulling(Camera & cam)
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
		if (DX::g_lights.at(i)->getDistanceFromCamera(cam) >= m_lightCullingDistance)
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
			temp.distance = DX::g_lights.at(i)->getDistanceFromCamera(cam);
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


void ForwardRender::_createConstantBuffer()
{
	HRESULT hr;
	hr = DXRHC::CreateConstantBuffer(this->m_objectBuffer, sizeof(ObjectBuffer));	
	hr = DXRHC::CreateConstantBuffer(this->m_cameraBuffer, sizeof(CameraBuffer));
	hr = DXRHC::CreateConstantBuffer(this->m_lightBuffer, sizeof(LightBuffer));
}


void ForwardRender::_createSamplerState()
{
	HRESULT hr = DXRHC::CreateSamplerState(m_samplerState);
	
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
}

void ForwardRender::_mapObjectBuffer(Drawable * drawable)
{
	m_objectValues.worldMatrix = drawable->getWorldmatrix();
	DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 0, 1, ShaderTypes::vertex);
}

void ForwardRender::_mapCameraBufferToVertex(Camera & camera)
{
	m_cameraValues.cameraPosition = camera.getPosition();
	m_cameraValues.viewProjection = camera.getViewProjection();
	DXRHC::MapBuffer(m_cameraBuffer, &m_cameraValues, sizeof(CameraBuffer), 1, 1, ShaderTypes::vertex);
}
void ForwardRender::_mapCameraBufferToPixel(Camera & camera)
{
	m_cameraValues.cameraPosition = camera.getPosition();
	m_cameraValues.viewProjection = camera.getViewProjection();
	DXRHC::MapBuffer(m_cameraBuffer, &m_cameraValues, sizeof(CameraBuffer), 2, 1, ShaderTypes::pixel);
}
void ForwardRender::_mapLightInfoNoMatrix()
{
	m_lightValues.info = DirectX::XMINT4((int32_t)DX::g_lights.size(), 0, 0, 0);
	for (unsigned int i = 0; i < DX::g_lights.size(); i++)
	{
		m_lightValues.position[i] = DX::g_lights[i]->getPosition();
		m_lightValues.color[i] = DX::g_lights[i]->getColor();
		m_lightValues.dropOff[i] = DirectX::XMFLOAT4A(DX::g_lights[i]->getIntensity(), DX::g_lights[i]->getDropOff(), DX::g_lights[i]->getPow(),0);
	}
	for (unsigned int i = (unsigned int)DX::g_lights.size(); i < 8; i++)
	{
		m_lightValues.position[i] = DirectX::XMFLOAT4A(0,0,0,0);
		m_lightValues.color[i] = DirectX::XMFLOAT4A(0,0,0,0);
		m_lightValues.dropOff[i] = DirectX::XMFLOAT4A(0, 0, 0, 0);
	}

	DXRHC::MapBuffer(m_lightBuffer, &m_lightValues, sizeof(LightBuffer), 0, 1, ShaderTypes::pixel);
}

void ForwardRender::_setStaticShaders()
{
	if (DX::g_geometryQueue.empty())
		return;
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DX::g_geometryQueue[0]->getVertexPath()));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(DX::g_geometryQueue[0]->getVertexPath()), nullptr, 0);
	

	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	if (m_lastPixelPath != DX::g_geometryQueue[0]->getPixelPath())
	{
		DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(DX::g_geometryQueue[0]->getPixelPath()), nullptr, 0);
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
		DX::g_deviceContext->Draw(DX::g_visabilityDrawQueue[i]->getVertexSize(), 0);
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

void ForwardRender::_setAnimatedShaders()
{
	if (DX::g_animatedGeometryQueue.size() > 0)
	{
		DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DX::g_animatedGeometryQueue[0]->getVertexPath()));
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(DX::g_animatedGeometryQueue[0]->getVertexPath()), nullptr, 0);
		DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
		DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
		DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
		if (m_lastPixelPath != DX::g_animatedGeometryQueue[0]->getPixelPath())
		{
			DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(DX::g_animatedGeometryQueue[0]->getPixelPath()), nullptr, 0);
		}
	}
	
}

void ForwardRender::_createUAV()
{
	HRESULT hr;
	hr = DXRHC::CreateTexture2D(m_uavTextureBuffer, 1, 2, D3D11_BIND_UNORDERED_ACCESS, 1, 1, 0, 1, 0, 0, DXGI_FORMAT_R8G8B8A8_TYPELESS);
	hr = DXRHC::CreateTexture2D(m_uavTextureBufferCPU, 1, 2, 0, 1, 1, 0, 1, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, 0, DXGI_FORMAT_R8G8B8A8_TYPELESS, D3D11_USAGE_STAGING);


	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	ZeroMemory(&UAVdesc, sizeof(UAVdesc));
	UAVdesc.Format = DXGI_FORMAT_R32_UINT;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Texture2D.MipSlice = 0;
	hr = DX::g_device->CreateUnorderedAccessView(m_uavTextureBuffer, &UAVdesc, &m_visabilityUAV);

}

void ForwardRender::_createShaders()
{
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

	m_shaderThreads[2] = std::thread(&ForwardRender::_createShadersInput, this);
	//shaderThreads[0] = std::thread([](D3D11_INPUT_ELEMENT_DESC animatedInputDesc[]) { DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl", "main", animatedInputDesc, 6);  });
	//DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl", "main", animatedInputDesc, 6);
	//DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");

	//DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 4);
	//shaderThreads[1] = std::thread([](D3D11_INPUT_ELEMENT_DESC inputDesc[]) {DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 4); });

	//DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");
	m_shaderThreads[0] = std::thread([]() { DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");  });

	//DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl");
	m_shaderThreads[1] = std::thread([]() {DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl");  });
	
	//DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl");

	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl");


}

void ForwardRender::_createShadersInput()
{
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

	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl", "main", animatedInputDesc, 6);

	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 4);
}
