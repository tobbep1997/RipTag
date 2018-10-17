#include "ForwardRender.h"
#include "../../Extern.h"
#include "../../../Shader/ShaderManager.h"
#include "../../RenderingManager.h"
#include "../Framework/DirectXRenderingHelpClass.h"
#include "../../../Helper/Timer.h"


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
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	_createShaders();
	
	_createConstantBuffer();
	_createSamplerState();
	m_shadowMap.Init(128, 128);
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc,	sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr = DX::g_device->CreateBlendState(&omDesc, &m_alphaBlend);

	m_visabilityPass.Init();


	DX::g_deviceContext->RSGetState(&m_standardRast);

	D3D11_RASTERIZER_DESC wfdesc{};
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	DX::g_device->CreateRasterizerState(&wfdesc, &m_wireFrame);
	DX::g_deviceContext->RSSetState(m_wireFrame);

	
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_NONE;
	//wfdesc.DepthClipEnable = FALSE;
	DX::g_device->CreateRasterizerState(&wfdesc, &m_disableBackFace);
	DX::g_deviceContext->RSSetState(m_disableBackFace);

	m_animationBuffer.SetAnimationCBuffer();

	m_2DRender.Init();
}

void ForwardRender::GeometryPass()
{
	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);
	if (m_firstRun == true)
	{
		m_shaderThreads[0].join();
		//m_shaderThreads[1].join();
		m_shaderThreads[2].join();
		m_firstRun = false;
	}
	
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	_setStaticShaders();
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getEntityType() != EntityType::PlayerType)
		{
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

			_mapObjectBuffer(DX::g_geometryQueue[i]);
			DX::g_geometryQueue[i]->BindTextures();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
		}
		
		
	}

	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
}

void ForwardRender::AnimatedGeometryPass()
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;
	_setAnimatedShaders();
	for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
	{
		if (DX::g_animatedGeometryQueue[i]->getEntityType() != EntityType::PlayerType)
		{
			ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

			_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);
		
			DX::g_animatedGeometryQueue[i]->BindTextures();

			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			_mapSkinningBuffer(DX::g_animatedGeometryQueue[i]);
			DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
		}		
	}
}

void ForwardRender::Flush(Camera & camera)
{
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	_simpleLightCulling(camera);
	this->m_shadowMap.MapAllLightMatrix(&DX::g_lights);
	_mapLightInfoNoMatrix();
	this->m_shadowMap.ShadowPass(&m_animationBuffer);
	this->m_shadowMap.SetSamplerAndShaderResources();
	VisabilityPass();
	_mapCameraBuffer(camera);
	this->GeometryPass();
	this->AnimatedGeometryPass();
	this->_wireFramePass();
	static bool drawFrustum = true;
	if (InputHandler::isKeyPressed('J'))
	{
		drawFrustum = false;
	}
	else if (InputHandler::isKeyPressed('K'))
	{
		drawFrustum = true;
	}

	if (drawFrustum)
		_GuardFrustumDraw();
	m_2DRender.GUIPass();
}

void ForwardRender::Clear()
{
	float c[4] = { 0.0f,0.0f,0.0f,1.0f };

	

	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_animatedGeometryQueue.clear();
	DX::g_lights.clear();

	DX::g_geometryQueue.clear();

	DX::g_visabilityDrawQueue.clear();

	this->m_shadowMap.Clear();
	DX::g_visibilityComponentQueue.clear();
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
	DX::SafeRelease(m_textureBuffer);

	DX::SafeRelease(m_alphaBlend);
	DX::SafeRelease(m_GuardBuffer);

	DX::SafeRelease(m_standardRast);
	DX::SafeRelease(m_wireFrame);

	DX::SafeRelease(m_disableBackFace);

	DX::SafeRelease(m_shadowSampler);
	m_shadowMap.Release();
	m_2DRender.Release();
}

void ForwardRender::_GuardFrustumDraw()
{
	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);

	DX::g_deviceContext->RSSetState(m_disableBackFace);

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumPixel.hlsl"), nullptr, 0);

	for (unsigned int i = 0; i < DX::g_visibilityComponentQueue.size(); i++)
	{

		DirectX::XMFLOAT4X4A viewProj = DX::g_visibilityComponentQueue[i]->getCamera()->getViewProjection();
		DirectX::XMMATRIX mViewProj = DirectX::XMLoadFloat4x4A(&viewProj);
		DirectX::XMVECTOR d = DirectX::XMMatrixDeterminant(mViewProj);
		DirectX::XMMATRIX mViewProjInverse = DirectX::XMMatrixInverse(&d, mViewProj);


		D3D11_MAPPED_SUBRESOURCE dataPtr;
		GuardBuffer gb;
		DirectX::XMStoreFloat4x4A(&gb.viewProj, mViewProj);
		DirectX::XMStoreFloat4x4A(&gb.viewProjInverse, mViewProjInverse);
		//gb.worldMatrix = DX::g_visibilityComponentQueue[i]->getWorldMatrix();
		
		DX::g_deviceContext->Map(m_GuardBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
		// copy memory from CPU to GPU the entire struct
		memcpy(dataPtr.pData, &gb, sizeof(GuardBuffer));
		// UnMap constant buffer so that we can use it again in the GPU
		DX::g_deviceContext->Unmap(m_GuardBuffer, 0);
		// set resource to Vertex Shader
		DX::g_deviceContext->VSSetConstantBuffers(5, 1, &m_GuardBuffer);

		ID3D11Buffer * ver = DX::g_visibilityComponentQueue[i]->getFrustumBuffer();

		UINT32 sizeVertex = DX::g_visibilityComponentQueue[i]->sizeOfFrustumVertex();
		UINT32 offset = 0;

		DX::g_deviceContext->IASetVertexBuffers(0, 1, &ver, &sizeVertex, &offset);
		DX::g_deviceContext->Draw(DX::g_visibilityComponentQueue[i]->getFrustum()->size(), 0);

	}
	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
	DX::g_deviceContext->RSSetState(m_standardRast);
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
	hr = DXRHC::CreateConstantBuffer(this->m_GuardBuffer, sizeof(GuardBuffer));
	hr = DXRHC::CreateConstantBuffer(this->m_textureBuffer, sizeof(TextureBuffer));
}

void ForwardRender::_createSamplerState()
{
	HRESULT hr = DXRHC::CreateSamplerState(m_samplerState, D3D11_TEXTURE_ADDRESS_WRAP);
	hr = DXRHC::CreateSamplerState(m_shadowSampler);
	
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);
}

void ForwardRender::_mapObjectBuffer(Drawable * drawable)
{
	m_objectValues.worldMatrix = drawable->getWorldmatrix();	
	DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex);
	


	m_textureValues.textureTileMult.x = drawable->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = drawable->getTextureTileMult().y;

	m_textureValues.usingTexture.x = drawable->isTextureAssigned();

	m_textureValues.color = drawable->getColor();

	DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel);
}

void ForwardRender::_mapSkinningBuffer(Drawable * drawable)
{
	//if (!m_animationBuffer)
	//{
	//	//m_animationBuffer = std::make_unique<Animation::AnimationCBuffer>();
	//	m_animationBuffer = new Animation::AnimationCBuffer();
	//	m_animationBuffer->SetAnimationCBuffer();
	//}

	std::vector<DirectX::XMFLOAT4X4A> skinningVector = drawable->getAnimatedModel()->GetSkinningMatrices();

	m_animationBuffer.UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	m_animationBuffer.SetToShader();
}

void ForwardRender::_mapCameraBuffer(Camera & camera)
{
	m_cameraValues.cameraPosition = camera.getPosition();
	m_cameraValues.viewProjection = camera.getViewProjection();
	DXRHC::MapBuffer(m_cameraBuffer, &m_cameraValues, sizeof(CameraBuffer), 2, 1, ShaderTypes::vertex);
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
	m_visabilityPass.SetViewportAndRenderTarget();
	for (VisibilityComponent * guard : DX::g_visibilityComponentQueue)
	{
		m_visabilityPass.GuardDepthPrePassFor(guard, &m_animationBuffer);
		m_visabilityPass.CalculateVisabilityFor(guard, &m_animationBuffer);
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
	D3D11_INPUT_ELEMENT_DESC guardFrustumInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ALPHA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	//TODO:: ADD NEW SHADER FOR ANIMATED OBJECTS, CONNECT TOANIMATEDINPUTDESC

	m_shaderThreads[2] = std::thread(&ForwardRender::_createShadersInput, this);
	m_shaderThreads[0] = std::thread([]() {DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl");  });
	//m_shaderThreads[1] = std::thread([]() {DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl");  });

	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowVertexAnimated.hlsl");
	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumVertex.hlsl", "main", guardFrustumInputDesc, 3);
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumPixel.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertex.hlsl");

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

	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl", "main", animatedInputDesc, 6);

	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl", "main", inputDesc, 4);
}

void ForwardRender::_wireFramePass()
{
	DX::g_deviceContext->RSSetState(m_wireFrame);

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	_setStaticShaders();
	for (unsigned int i = 0; i < DX::g_wireFrameDrawQueue.size(); i++)
	{
		ID3D11Buffer * vertexBuffer = DX::g_wireFrameDrawQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_wireFrameDrawQueue[i]);
		DX::g_wireFrameDrawQueue[i]->BindTextures();
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_wireFrameDrawQueue[i]->getVertexSize(), 0);

	}

	DX::g_deviceContext->RSSetState(m_standardRast);
}


