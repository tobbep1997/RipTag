#include "EnginePCH.h"
#include "ForwardRender.h"
#include "2D Engine/DirectXTK/ScreenGrab.h"
#include <wincodec.h>
#include <filesystem>


ForwardRender::ForwardRender()
{
	m_lastVertexPath = L"NULL";
	m_lastPixelPath = L"NULL";

	m_visabilityPass = new VisabilityPass();

}


ForwardRender::~ForwardRender()
{
	
	
}

void ForwardRender::Init(IDXGISwapChain * swapChain,
	ID3D11RenderTargetView * backBufferRTV,
	ID3D11DepthStencilView * depthStencilView,
	ID3D11DepthStencilState* m_depthStencilState,
	ID3D11Texture2D * depthBufferTex,
	ID3D11SamplerState * samplerState,
	D3D11_VIEWPORT viewport,
	const WindowContext & windowContext)
{
	m_swapChain = swapChain;
	m_backBufferRTV = backBufferRTV;
	m_depthStencilView = depthStencilView;
	m_depthBufferTex = depthBufferTex;
	m_samplerState = samplerState;
	m_viewport = viewport;
	this->m_depthStencilState = m_depthStencilState;
	float c[4] = { 1.0f,0.0f,1.0f,1.0f };


	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	
	_createShaders();

	_InitScreenShoot();
	_createConstantBuffer();
	_createSamplerState();

	_OutlineDepthCreate();
	m_shadowMap = new ShadowMap();
	switch (windowContext.graphicsQuality)
	{
	case 0:
		m_shadowMap->Init(32, 32);
		m_lightCullingDistance = 50;
		m_forceCullingLimit = 4;
		break;
	case 1:
		m_shadowMap->Init(128, 128);
		m_lightCullingDistance = 50;
		m_forceCullingLimit = 4;
		break;
	case 2:
		m_shadowMap->Init(1024, 1024);
		m_lightCullingDistance = 100;
		m_forceCullingLimit = 8;
		break;
	case 3:
		m_shadowMap->Init(2048, 2048);
		m_lightCullingDistance = 250;
		m_forceCullingLimit = 8;
		break;
	default:
		m_shadowMap->Init(64, 64);
		m_lightCullingDistance = 50;
		m_forceCullingLimit = 5;
		break;
	}
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable = true;
	omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateBlendState(&omDesc, &m_alphaBlend)))
	{
		DX::SetName(m_alphaBlend, "ForwardRender: m_alphaBlend");
	}

	
	m_visabilityPass->Init();


	DX::g_deviceContext->RSGetState(&m_standardRast);

	D3D11_RASTERIZER_DESC wfdesc{};
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	if (SUCCEEDED(hr = DX::g_device->CreateRasterizerState(&wfdesc, &m_wireFrame)))
	{
		DX::SetName(m_wireFrame, "ForwardRender: m_wireFrame");
		DX::g_deviceContext->RSSetState(m_wireFrame);
	}


	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_SOLID;
	wfdesc.CullMode = D3D11_CULL_NONE;
	if (SUCCEEDED(DX::g_device->CreateRasterizerState(&wfdesc, &m_disableBackFace)))
	{
		DX::SetName(m_disableBackFace, "ForwardRender: m_disableBackFace");
		DX::g_deviceContext->RSSetState(m_disableBackFace);
	}

	if (SUCCEEDED(hr = DXRHC::CreateRasterizerState("m_NUKE",m_NUKE, FALSE, D3D11_CULL_NONE, 0, 0, FALSE)))
	{
		
	}

	m_animationBuffer = new Animation::AnimationCBuffer();
	m_animationBuffer->SetAnimationCBuffer();

	m_2DRender = new Render2D();
	m_2DRender->Init();

	D3D11_DEPTH_STENCIL_DESC dpd{};
	dpd.DepthEnable = TRUE;
	dpd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dpd.DepthFunc = D3D11_COMPARISON_LESS;

	//Create the Depth/Stencil View
	if (SUCCEEDED(DX::g_device->CreateDepthStencilState(&dpd, &m_particleDepthStencilState)))
	{
		DX::SetName(m_particleDepthStencilState, "m_particleDepthStencilState");
	}

}

void ForwardRender::GeometryPass(Camera & camera)
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);
	
	
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/VertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	//_setStaticShaders();

	DirectX::BoundingFrustum * bf = _createBoundingFrustrum(&camera);
	for (int i = 0; i < DX::g_cullQueue.size(); i++)
	{
		switch (camera.getPerspectiv())
		{
		case Camera::Perspectiv::Player:
			if (DX::g_cullQueue[i]->getEntityType() == EntityType::PlayerType)						
				continue;			
			break;
		case Camera::Perspectiv::Enemy:
			if (DX::g_cullQueue[i]->getEntityType() == EntityType::GuarddType && DX::g_cullQueue[i]->getEntityType() == EntityType::FirstPersonPlayer)
				continue;
			break;
		default:
			break;
		}

		if (DX::g_cullQueue[i]->getBoundingBox())
		{
			if (_Cull(bf, DX::g_cullQueue[i]->getBoundingBox()))
				continue;
		}
		DX::INSTANCING::tempInstance(DX::g_cullQueue[i]);
	}
	delete bf;
	DrawInstancedCull(&camera, true);



	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
}

void ForwardRender::PrePass(Camera & camera)
{
	//THIS IS VERY FUCKING IMPORTANT 
	//THIS WILL KILL EVERYTHING
	if (m_firstRun == true)
	{
		m_shaderThreads[0].join();
		//m_shaderThreads[1].join();
		m_shaderThreads[2].join();
		m_firstRun = false;
	}
	//DONT FUCKING TOUCH
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/VertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);	
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);


	DirectX::BoundingFrustum * bf = _createBoundingFrustrum(&camera);

	for (int i = 0; i < DX::g_cullQueue.size(); i++)
	{
		if (DX::g_cullQueue[i]->getEntityType() != EntityType::PlayerType)
		{
			if (DX::g_cullQueue[i]->getBoundingBox())
			{
				if (_Cull(bf, DX::g_cullQueue[i]->getBoundingBox()))
					continue;
			}
			DX::INSTANCING::tempInstance(DX::g_cullQueue[i]);

		}
	}
	delete bf;
	DrawInstancedCull(&camera, false);



	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/OutlinePrepassVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePrepassVertex.hlsl"), nullptr, 0);

	UINT32 vertexSize = sizeof(StaticVertex);

	UINT32 offset = 0;
	for (int i = 0; i < DX::g_outlineQueue.size(); i++)
	{
		{
			DX::g_deviceContext->OMSetDepthStencilState(m_write0State, 0);
			ID3D11Buffer * vertexBuffer = DX::g_outlineQueue[i]->getBuffer();

			_mapObjectOutlineBuffer(DX::g_outlineQueue[i], camera.getPosition());
			DX::g_outlineQueue[i]->BindTextures();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_outlineQueue[i]->getVertexSize(), 0);

		}

		{

			DX::g_deviceContext->OMSetDepthStencilState(m_write1State, 0);
			ID3D11Buffer * vertexBuffer = DX::g_outlineQueue[i]->getBuffer();

			_mapObjectInsideOutlineBuffer(DX::g_outlineQueue[i], camera.getPosition());
			DX::g_outlineQueue[i]->BindTextures();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_outlineQueue[i]->getVertexSize(), 0);

		}

		DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}


	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
}

void ForwardRender::AnimationPrePass(Camera& camera)
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/PreAnimatedVertexShader.hlsl"));
	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;
	DX::g_deviceContext->RSSetState(m_NUKE);
	DX::g_deviceContext->OMSetDepthStencilState(m_NUKE2, 0);

	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/PreAnimatedVertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);

	DirectX::BoundingFrustum * bf = _createBoundingFrustrum(&camera);
	for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
	{
		if (DX::g_animatedGeometryQueue[i]->getEntityType() != EntityType::FirstPersonPlayer)
			if (_Cull(bf, DX::g_animatedGeometryQueue[i]->getBoundingBox()))
				continue;
		

		auto lol = DX::g_animatedGeometryQueue.at(i)->GetUAV();
		DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
			0,
			nullptr,
			NULL,
			//NULL,
			0, 1, &lol, 0
		);

		ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);

		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		_mapSkinningBuffer(DX::g_animatedGeometryQueue[i]);
		DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);


		
	}
	delete bf;
	DX::g_deviceContext->RSSetState(m_standardRast);
	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);

}

void ForwardRender::AnimatedGeometryPass(Camera & camera)
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	UINT32 vertexSize = sizeof(PostAniDynamicVertex);
	UINT32 offset = 0;
	//_setAnimatedShaders();

	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl"), nullptr, 0);

	DirectX::BoundingFrustum * bf = _createBoundingFrustrum(&camera);
	for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
	{
		if (DX::g_animatedGeometryQueue[i]->getEntityType() != EntityType::FirstPersonPlayer)
			if (_Cull(bf, DX::g_animatedGeometryQueue[i]->getBoundingBox()))
				continue;
		if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::FirstPersonPlayer)
		{
			DX::g_deviceContext->OMSetDepthStencilState(m_write1State, 0);
		}
		if (DX::g_animatedGeometryQueue[i]->getHidden() != true)
		{
			//ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

			switch (camera.getPerspectiv())
			{
			case Camera::Perspectiv::Player:
				if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::PlayerType)
					continue;
				break;
			case Camera::Perspectiv::Enemy:
				if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::CurrentGuard || DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::FirstPersonPlayer)
					continue;
				break;
			}

			ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->GetAnimatedVertex();

			_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);

			if (DX::g_animatedGeometryQueue[i]->getTextureName().find("DOOR") <= DX::g_animatedGeometryQueue[i]->getTextureName().size())
			{
				int gfaghjk34h = 5;
			}

			DX::g_animatedGeometryQueue[i]->BindTextures();

			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			//_mapSkinningBuffer(DX::g_animatedGeometryQueue[i]);
			DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);

			//DX::g_animatedGeometryQueue[i]->TEMP();
			if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::FirstPersonPlayer)
			{
				DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
			}
		}		
	}
	delete bf;
}

void ForwardRender::Flush(Camera & camera)
{
	if (DX::g_screenShootCamera == true)
	{
		this->FlushScreenShoot(camera);
		return;
	}

	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, NULL);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);
	this->AnimationPrePass(camera);

	Camera * dbg_camera = new Camera(DirectX::XM_PI * 0.75f, 16.0f / 9.0f, 1, 100);
	dbg_camera->setDirection(0, -1, 0);
	dbg_camera->setUP(1, 0, 0);
	DirectX::XMFLOAT4A pos = camera.getPosition();
	pos.y += 10;
	dbg_camera->setPosition(pos);

	if (Cheet::g_DBG_CAM)
	{
		_mapCameraBuffer(*dbg_camera);
	}
	//_mapCameraBuffer(*dbg_camera);
	this->PrePass(camera);
	
	
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);
	_simpleLightCulling(camera);

	_mapLightInfoNoMatrix();
	shadowRun++;
	if (shadowRun % 2 == 0 || true)
	{
		this->m_shadowMap->ShadowPass(this);
		shadowRun = 0;
	}
	else
		this->m_shadowMap->MapAllLightMatrix(&DX::g_prevlights);
	this->m_shadowMap->SetSamplerAndShaderResources();

	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	DX::g_deviceContext->RSSetState(m_standardRast);

	if (DX::g_player)
		_visabilityPass();

	if (Cheet::g_DBG_CAM)
	{
		_mapCameraBuffer(*dbg_camera);
	}
	//_mapCameraBuffer(*dbg_camera);
	this->GeometryPass(camera);
	this->AnimatedGeometryPass(camera);
	this->_OutliningPass(camera);


	//_GuardFrustumDraw();
	if (Cheet::g_DBG_CAM)
	{
		_DBG_DRAW_CAMERA(camera);
	}
	//_DBG_DRAW_CAMERA(camera);
	_mapCameraBuffer(camera);
	
	_particlePass(&camera);

	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	m_2DRender->GUIPass();
	this->_wireFramePass(&camera);
	
	delete dbg_camera;
}

void ForwardRender::Clear()
{
	float c[4] = { .5f,.5f,.5f,1.0f };
	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_animatedGeometryQueue.clear();
	DX::g_lights.clear();

	DX::g_player = nullptr;
	DX::g_remotePlayer = nullptr;

	DX::g_outlineQueue.clear();
	DX::g_visibilityComponentQueue.clear();

	DX::g_wireFrameDrawQueue.clear();

	DX::INSTANCING::g_instanceGroups.clear();
	DX::INSTANCING::g_instanceShadowGroups.clear();
	DX::INSTANCING::g_instanceWireFrameGroups.clear();
	DX::g_cullQueue.clear();

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

	DX::SafeRelease(m_write0State);
	DX::SafeRelease(m_write1State);
	DX::SafeRelease(m_OutlineState);
	//DX::SafeRelease(depthoutState);
	DX::SafeRelease(m_particleDepthStencilState);
	DX::SafeRelease(m_outlineBuffer);

	DX::SafeRelease(m_NUKE);
	DX::SafeRelease(m_NUKE2);

	//Release ScreenCap
	DX::SafeRelease(m_screenShootRender);
	DX::SafeRelease(m_screenShootTex);
	DX::SafeRelease(m_screenShootSRV);

	m_shadowMap->Release();
	delete m_shadowMap;

	m_2DRender->Release();
	delete m_2DRender;
	delete m_animationBuffer;

	delete m_visabilityPass;
}

void ForwardRender::DrawInstanced(Camera* camera, std::vector<DX::INSTANCING::GROUP> * instanceGroup, const bool& bindTextures)
{
	using namespace DirectX;
	using namespace DX::INSTANCING;
	if (camera)
		_mapCameraBuffer(*camera);

	size_t instanceGroupSize = instanceGroup->size();
	size_t attributeSize = 0;
	ID3D11Buffer * instanceBuffer;
	for (size_t group = 0; group < instanceGroupSize; group++)
	{
		GROUP instance = instanceGroup->at(group);

		D3D11_BUFFER_DESC instBuffDesc;
		memset(&instBuffDesc, 0, sizeof(instBuffDesc));
		instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		instBuffDesc.ByteWidth = sizeof(OBJECT) * (UINT)instance.attribs.size();
		instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA instData;
		memset(&instData, 0, sizeof(instData));
		instData.pSysMem = instance.attribs.data();
		HRESULT hr;
		if (SUCCEEDED(hr = DX::g_device->CreateBuffer(&instBuffDesc, &instData, &instanceBuffer)))
		{
			DX::SetName(instanceBuffer, "instanceBuffer");
		}
		//We copy the data into the attribute part of the layout.
		// makes instancing special

		//Map Texture
		//-----------------------------------------------------------
		if (bindTextures)
		{
			std::string textureName = instance.textureName;
			size_t t = textureName.find_last_of('/');
			textureName = textureName.substr(t + 1);
			Manager::g_textureManager.getTexture(textureName)->Bind(1);
		}
		//-----------------------------------------------------------


		UINT offset = 0;
		ID3D11Buffer * bufferPointers[2];
		bufferPointers[0] = instance.staticMesh->getBuffer();
		bufferPointers[1] = instanceBuffer;

		unsigned int strides[2];
		strides[0] = sizeof(StaticVertex);
		strides[1] = sizeof(OBJECT);

		unsigned int offsets[2];
		offsets[0] = 0;
		offsets[1] = 0;

		DX::g_deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

		DX::g_deviceContext->DrawInstanced(instance.staticMesh->getVertice().size(),
			instance.attribs.size(),
			0U,
			0U);
		DX::SafeRelease(instanceBuffer);
		//DX::SafeRelease(bufferPointers[0]);
		//DX::SafeRelease(bufferPointers[1]);
	}
}

void ForwardRender::DrawInstancedCull(Camera* camera, const bool& bindTextures)
{
	using namespace DirectX;
	using namespace DX::INSTANCING;
	if (camera)//TODO TAKE ME HOME
		_mapCameraBuffer(*camera);
	   
	size_t instanceGroupSize = g_temp.size();
	size_t attributeSize = 0;
	ID3D11Buffer * instanceBuffer;
	for (size_t group = 0; group < instanceGroupSize; group++)
	{
		GROUP instance = g_temp.at(group);

		D3D11_BUFFER_DESC instBuffDesc;
		memset(&instBuffDesc, 0, sizeof(instBuffDesc));
		instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		instBuffDesc.ByteWidth = sizeof(OBJECT) * (UINT)instance.attribs.size();
		instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA instData;
		memset(&instData, 0, sizeof(instData));
		instData.pSysMem = instance.attribs.data();
		HRESULT hr;
		if (SUCCEEDED(hr = DX::g_device->CreateBuffer(&instBuffDesc, &instData, &instanceBuffer)))
		{
			DX::SetName(instanceBuffer, "instanceBuffer");
		}
		//We copy the data into the attribute part of the layout.
		// makes instancing special

		//Map Texture
		//-----------------------------------------------------------
		if (bindTextures)
		{
			std::string textureName = instance.textureName;
			size_t t = textureName.find_last_of('/');
			textureName = textureName.substr(t + 1);
			Manager::g_textureManager.getTexture(textureName)->Bind(1);
		}
		//-----------------------------------------------------------


		UINT offset = 0;
		ID3D11Buffer * bufferPointers[2];
		bufferPointers[0] = instance.staticMesh->getBuffer();
		bufferPointers[1] = instanceBuffer;

		unsigned int strides[2];
		strides[0] = sizeof(StaticVertex);
		strides[1] = sizeof(OBJECT);

		unsigned int offsets[2];
		offsets[0] = 0;
		offsets[1] = 0;

		DX::g_deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

		DX::g_deviceContext->DrawInstanced(instance.staticMesh->getVertice().size(),
			instance.attribs.size(),
			0U,
			0U);
		DX::SafeRelease(instanceBuffer);
	}
	g_temp.clear();
}

ID3D11BlendState * ForwardRender::getAlphaBlendState()
{
	return m_alphaBlend; 
}

ID3D11DepthStencilState * ForwardRender::getDepthStencilState()
{
	return m_depthStencilState; 
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

void ForwardRender::_DBG_DRAW_CAMERA(Camera& camera)
{
	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);

	DX::g_deviceContext->RSSetState(m_disableBackFace);

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumPixel.hlsl"), nullptr, 0);


	DirectX::XMFLOAT4X4A viewProj = camera.getViewProjection();
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

	VisibilityComponent * tmp = new VisibilityComponent();
	tmp->Init(&camera);

	ID3D11Buffer * ver = tmp->getFrustumBuffer();

	UINT32 sizeVertex = tmp->sizeOfFrustumVertex();
	UINT32 offset = 0;

	DX::g_deviceContext->IASetVertexBuffers(0, 1, &ver, &sizeVertex, &offset);
	DX::g_deviceContext->Draw(tmp->getFrustum()->size(), 0);

	delete tmp;
	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
	DX::g_deviceContext->RSSetState(m_standardRast);
}

void ForwardRender::_simpleLightCulling(Camera & cam)
{
	float culled = 0;
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
		//-----------------------------
		///Calculating the distance of the lights
		//The SortStruct can be removed but not know
		for (size_t i = 0; i < DX::g_lights.size(); i++)
		{
			DX::g_lights[i]->setDistanceToCamera(DX::g_lights.at(i)->getDistanceFromCamera(cam));
		}
		//
		//------------------------------
		///BubbleSorting the values. 1->...->100000
		//
		bool sorted = false;
		while (false == sorted)
		{
			sorted = true;
			for (int i = 0; i < DX::g_lights.size() - 1; ++i)
			{
				if (DX::g_lights[i]->getDistanceToCamera() > DX::g_lights[i + 1]->getDistanceToCamera())
				{
					sorted = false;
					PointLight * swap = DX::g_lights[i];
					DX::g_lights.at(i) = DX::g_lights.at(i + 1);
					DX::g_lights.at(i + 1) = swap;

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


}

void ForwardRender::_GuardLightCulling()
{

}

void ForwardRender::_createConstantBuffer()
{
	HRESULT hr = 0;
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("ObjectBuffer", this->m_objectBuffer, sizeof(ObjectBuffer)))){}
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("CameraBuffer", this->m_cameraBuffer, sizeof(CameraBuffer)))) {}
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("LightBuffer", this->m_lightBuffer, sizeof(LightBuffer)))) {}
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("GuardBuffer", this->m_GuardBuffer, sizeof(GuardBuffer)))) {}
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("TextureBuffer", this->m_textureBuffer, sizeof(TextureBuffer)))) {}
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("OutLineBuffer", this->m_outlineBuffer, sizeof(OutLineBuffer)))) {}
}

void ForwardRender::_createSamplerState()
{
	HRESULT hr;
	if (SUCCEEDED(hr = DXRHC::CreateSamplerState("Default Sampler State", m_samplerState, D3D11_TEXTURE_ADDRESS_WRAP)))
	{
		DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);		
	}
	if (SUCCEEDED(hr = DXRHC::CreateSamplerState("Shadow Sampler State",m_shadowSampler)))
	{
		DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);		
	}
	
}

void ForwardRender::_mapObjectBuffer(Drawable * drawable)
{
	HRESULT hr;
	m_objectValues.worldMatrix = drawable->getWorldmatrix();
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex))) { }
	


	m_textureValues.textureTileMult.x = drawable->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = drawable->getTextureTileMult().y;

	m_textureValues.usingTexture.x = drawable->isTextureAssigned();
	if (drawable->getTexture()->getIndex() != -1)
	{
		m_textureValues.usingTexture.y = -1;
		m_textureValues.usingTexture.z = drawable->getTexture()->getIndex();
	}
	else
	{
		m_textureValues.usingTexture.y = -1;
		m_textureValues.usingTexture.z = drawable->getTexture()->getIndex();
	}

	m_textureValues.color = drawable->getColor();

	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel))) { }
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::vertex))) { }
}

void ForwardRender::_mapObjectOutlineBuffer(Drawable* drawable, const DirectX::XMFLOAT4A & pos)
{
	HRESULT hr;
	m_objectValues.worldMatrix = drawable->getWorldMatrixForOutline(pos);
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex))) { }



	m_textureValues.textureTileMult.x = drawable->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = drawable->getTextureTileMult().y;

	m_textureValues.usingTexture.x = drawable->isTextureAssigned();

	m_textureValues.color = DirectX::XMFLOAT4A(2000,0,0,1);

	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel))) { }
}

void ForwardRender::_mapObjectInsideOutlineBuffer(Drawable* drawable, const DirectX::XMFLOAT4A& pos)
{
	HRESULT hr;
	m_objectValues.worldMatrix = drawable->getWorldMatrixForInsideOutline(pos);
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex))) { }



	m_textureValues.textureTileMult.x = drawable->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = drawable->getTextureTileMult().y;

	m_textureValues.usingTexture.x = drawable->isTextureAssigned();

	m_textureValues.color = drawable->getColor();

	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel))) { }
}

void ForwardRender::_mapSkinningBuffer(Drawable * drawable)
{
	std::vector<DirectX::XMFLOAT4X4A> skinningVector = drawable->getAnimationPlayer()->GetSkinningMatrices();

	m_animationBuffer->UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	m_animationBuffer->SetToShader();
}

void ForwardRender::_mapCameraBuffer(Camera & camera)
{
	HRESULT hr;
	m_cameraValues.cameraInfo = DirectX::XMFLOAT4A(camera.getNearPlane(), camera.getFarPlane(), 0, 0);
	m_cameraValues.cameraPosition = camera.getPosition();
	m_cameraValues.viewProjection = camera.getViewProjection();
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_cameraBuffer, &m_cameraValues, sizeof(CameraBuffer), 2, 1, ShaderTypes::vertex))) { }
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_cameraBuffer, &m_cameraValues, sizeof(CameraBuffer), 2, 1, ShaderTypes::pixel))) { }
}

void ForwardRender::_mapLightInfoNoMatrix()
{
	HRESULT hr;
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

	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_lightBuffer, &m_lightValues, sizeof(LightBuffer), 0, 1, ShaderTypes::pixel))) { }
}

void ForwardRender::_OutliningPass(Camera & cam)
{
	HRESULT hr;
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/OutlinePrepassVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/OutlineVertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePixelShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	//DX::g_deviceContext->OMSetDepthStencilState(depthoutState, 0);
	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	//_setStaticShaders();
	//TODO
	for (unsigned int i = 0; i < DX::g_outlineQueue.size(); i++)
	{			
		m_outLineValues.outLineColor = DX::g_outlineQueue[i]->getOutlineColor();
		//DX::g_deviceContext->PSSetShaderResources(10, 1, &m_outlineShaderRes);

		if (FAILED(hr = DXRHC::MapBuffer(m_outlineBuffer, &m_outLineValues, sizeof(OutLineBuffer), 8, 1, ShaderTypes::pixel)))
		{
			continue;
		}

		DX::g_deviceContext->OMSetDepthStencilState(m_OutlineState, 0);

		ID3D11Buffer * vertexBuffer = DX::g_outlineQueue[i]->getBuffer();

		_mapObjectOutlineBuffer(DX::g_outlineQueue[i], cam.getPosition());
		DX::g_outlineQueue[i]->BindTextures();
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_outlineQueue[i]->getVertexSize(), 0);
			

	}

	_setAnimatedShaders();
	for (int i = 0; i < DX::g_animatedGeometryQueue.size(); ++i)
	{
		if (DX::g_animatedGeometryQueue[i]->getOutline() == true)
		{
			m_outLineValues.outLineColor = DX::g_animatedGeometryQueue[i]->getOutlineColor();
			//DX::g_deviceContext->PSSetShaderResources(10, 1, &m_outlineShaderRes);

			if (FAILED(hr = DXRHC::MapBuffer(m_outlineBuffer, &m_outLineValues, sizeof(OutLineBuffer), 8, 1, ShaderTypes::pixel)))
			{
				continue;
			}

			DX::g_deviceContext->OMSetDepthStencilState(m_OutlineState, 0);

			ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

			_mapObjectOutlineBuffer(DX::g_animatedGeometryQueue[i], cam.getPosition());
			DX::g_animatedGeometryQueue[i]->BindTextures();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
		}
	}

}

void ForwardRender::_OutlineDepthCreate()
{
//;
//	
//	= DX::g_device->CreateTexture2D(&depthStencilDesc, NULL, &m_outlineDepthBufferTex);
//	hr = DX::g_device->CreateDepthStencilView(m_outlineDepthBufferTex, NULL, &m_outlineDepthStencil);
	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC depth{};
	depth.DepthEnable = FALSE;
	depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth.StencilEnable = TRUE;

	depth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depth.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
	depth.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;

	depth.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	depth.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	
	depth.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;
	depth.BackFace.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;

	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilState(&depth, &m_write1State)))
	{
		DX::SetName(m_write1State, "m_write1State");
	}

	D3D11_DEPTH_STENCIL_DESC depth2{};
	depth2.DepthEnable = FALSE;
	depth2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth2.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth2.StencilEnable = TRUE;
		 
	depth2.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth2.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth2.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth2.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		 
	depth2.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR_SAT;
	depth2.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR_SAT;
		 
	depth2.FrontFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;
	depth2.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;
		 
	depth2.FrontFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;
	depth2.BackFace.StencilFailOp = D3D11_STENCIL_OP_DECR_SAT;

	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilState(&depth2, &m_write0State)))
	{
		DX::SetName(m_write0State, "m_write0State");
	}

	D3D11_DEPTH_STENCIL_DESC depth3{};
	depth2.DepthEnable = TRUE;
	depth2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth2.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth2.StencilEnable = TRUE;

	depth2.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depth2.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depth2.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
	depth2.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;

	depth2.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depth2.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	depth2.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth2.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	depth2.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth2.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilState(&depth2, &m_OutlineState)))
	{
		DX::SetName(m_OutlineState, "m_OutlineState");
	}

	D3D11_DEPTH_STENCIL_DESC depth4{};
	depth4.DepthEnable = FALSE;
	//depth2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//depth2.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth4.StencilEnable = FALSE;

	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilState(&depth4, &m_NUKE2)))
	{
		DX::SetName(m_NUKE2, "m_NUKE2");
	}

}

void ForwardRender::_visabilityPass()
{
	if (Cheet::g_visabilityDisabled)
	{
		return;
	}
	if (DX::g_player == nullptr)
	{
		return;
	}
	m_visabilityPass->SetViewportAndRenderTarget();
	for (VisibilityComponent * guard : DX::g_visibilityComponentQueue)
	{
		m_visabilityPass->GuardDepthPrePassFor(guard, this, m_animationBuffer);
		m_visabilityPass->CalculateVisabilityFor(guard, m_animationBuffer);
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

void ForwardRender::_particlePass(Camera * camera)
{
	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);
	DX::g_deviceContext->OMSetDepthStencilState(m_particleDepthStencilState, NULL);
	

	DirectX::XMMATRIX proj, viewInv;
	DirectX::BoundingFrustum boundingFrustum;
	proj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&camera->getProjection()));
	viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&camera->getView())));
	DirectX::BoundingFrustum::CreateFromMatrix(boundingFrustum, proj);
	boundingFrustum.Transform(boundingFrustum, viewInv);

	for (auto & emitter : DX::g_emitters)
	{
		if (boundingFrustum.Intersects(*emitter->getBoundingBox()))
		{
			emitter->Clear(); 
			emitter->Update(0,camera); 
			emitter->Draw();
			emitter->Clear(); 
		}
		else
			emitter->Clear();
	}
	DX::g_emitters.clear();
	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0);
	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, NULL);
}

void ForwardRender::_createShaders()
{

	//TODO:: ADD NEW SHADER FOR ANIMATED OBJECTS, CONNECT TOANIMATEDINPUTDESC

	m_shaderThreads[2] = std::thread(&ForwardRender::_createShadersInput, this);
	m_shaderThreads[0] = std::thread([]() {DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl");  });
	//m_shaderThreads[1] = std::thread([]() {DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityVertex.hlsl");  });

	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/VisabilityPixel.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowMap/ShadowVertexAnimated.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumPixel.hlsl");

	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePrepassVertex.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePixelShader.hlsl");

	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertex.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertexAnimated.hlsl");

	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/OutlineVertexShader.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/wireFramePixel.hlsl");

	
}

void ForwardRender::_createShadersInput()
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//World matrix
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		//Attributes
		{ "COLOR" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "UVMULT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INFO" , 0, DXGI_FORMAT_R32G32B32A32_SINT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	D3D11_INPUT_ELEMENT_DESC animatedInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTINFLUENCES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINTWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	D3D11_INPUT_ELEMENT_DESC postAnimatedInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC guardFrustumInputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ALPHA", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC outlineInputDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "UV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/Shaders/OutlinePrepassVertex.hlsl", "main", outlineInputDesc, 4);

	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl", "main", inputDesc, 11);

	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/PreAnimatedVertexShader.hlsl", "main", animatedInputDesc, 6);

	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl", "main", postAnimatedInputDesc, 4);

	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/Shaders/GuardFrustum/GuardFrustumVertex.hlsl", "main", guardFrustumInputDesc, 3);
}

void ForwardRender::_wireFramePass(Camera * camera)
{
	DX::g_deviceContext->RSSetState(m_wireFrame);

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/VertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	
	//_setStaticShaders();
	//Manager::g_textureManager.getTexture("BAR")->Bind(1);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/wireFramePixel.hlsl"), nullptr, 0);
	if (DX::INSTANCING::g_instanceWireFrameGroups.size() > 0)
		DrawInstanced(camera, &DX::INSTANCING::g_instanceWireFrameGroups, false);
	//for (unsigned int i = 0; i < DX::g_wireFrameDrawQueue.size(); i++)
	//{
	//	ID3D11Buffer * vertexBuffer = DX::g_wireFrameDrawQueue[i]->getBuffer();

	//	_mapObjectBuffer(DX::g_wireFrameDrawQueue[i]);
	//	DX::g_wireFrameDrawQueue[i]->BindTextures();
	//	DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	//	DX::g_deviceContext->Draw(DX::g_wireFrameDrawQueue[i]->getVertexSize(), 0);

	//}

	//DX::g_deviceContext->RSSetState(m_standardRast);
}

DirectX::BoundingFrustum * ForwardRender::_createBoundingFrustrum(Camera* camera)
{
	DirectX::XMMATRIX proj, viewInv;
	DirectX::BoundingFrustum * boundingFrustum = new DirectX::BoundingFrustum();
	proj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&camera->getProjection()));
	viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&camera->getView())));
	DirectX::BoundingFrustum::CreateFromMatrix(*boundingFrustum, proj);
	boundingFrustum->Transform(*boundingFrustum, viewInv);

	return boundingFrustum;
}

bool ForwardRender::_Cull(DirectX::BoundingFrustum* camera, DirectX::BoundingBox* box)
{
	return !camera->Intersects(*box);		
}

void ForwardRender::FlushScreenShoot(Camera& camera)
{

	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, NULL);
	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);
	this->AnimationPrePass(camera);

	Camera * dbg_camera = new Camera(DirectX::XM_PI * 0.75f, 16.0f / 9.0f, 1, 100);
	dbg_camera->setDirection(0, -1, 0);
	dbg_camera->setUP(1, 0, 0);
	DirectX::XMFLOAT4A pos = camera.getPosition();
	pos.y += 10;
	dbg_camera->setPosition(pos);
	//_mapCameraBuffer(*dbg_camera);
	this->PrePass(camera);


	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);
	_simpleLightCulling(camera);

	_mapLightInfoNoMatrix();
	shadowRun++;
	if (shadowRun % 2 == 0 || true)
	{
		this->m_shadowMap->ShadowPass(this);
		shadowRun = 0;
	}
	else
		this->m_shadowMap->MapAllLightMatrix(&DX::g_prevlights);
	this->m_shadowMap->SetSamplerAndShaderResources();

	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	DX::g_deviceContext->RSSetState(m_standardRast);

	if (DX::g_player)
		_visabilityPass();
	//------------------------------
	//Pass to Pic
	this->_GeometryPassToPic(camera);
	this->_AnimatedGeometryToPic(camera);
	//------------------------------
	//SaveFile to that folder
	HRESULT hr = DirectX::SaveWICTextureToFile(DX::g_deviceContext, m_screenShootTex, GUID_ContainerFormatDds , L"../Assets/GUIFOLDER/ENDGAME.DDS");
	

	this->_OutliningPass(camera);

	
	

	//_GuardFrustumDraw();
	//_DBG_DRAW_CAMERA(camera);
	_mapCameraBuffer(camera);

	_particlePass(&camera);

	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	m_2DRender->GUIPass();
	this->_wireFramePass(&camera);

	delete dbg_camera;
}

void ForwardRender::_GeometryPassToPic(Camera& camera)
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);


	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/VertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_screenShootRender, m_depthStencilView);
	//_setStaticShaders();

	DirectX::BoundingFrustum * bf = _createBoundingFrustrum(&camera);
	for (int i = 0; i < DX::g_cullQueue.size(); i++)
	{
		switch (camera.getPerspectiv())
		{
		case Camera::Perspectiv::Player:
			if (DX::g_cullQueue[i]->getEntityType() == EntityType::PlayerType)
				continue;
			break;
		case Camera::Perspectiv::Enemy:
			if (DX::g_cullQueue[i]->getEntityType() == EntityType::GuarddType && DX::g_cullQueue[i]->getEntityType() == EntityType::FirstPersonPlayer)
				continue;
			break;
		default:
			break;
		}

		if (DX::g_cullQueue[i]->getBoundingBox())
		{
			if (_Cull(bf, DX::g_cullQueue[i]->getBoundingBox()))
				continue;
		}
		DX::INSTANCING::tempInstance(DX::g_cullQueue[i]);
	}
	delete bf;
	DrawInstancedCull(&camera, true);



	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
}

void ForwardRender::_AnimatedGeometryToPic(Camera& camera)
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_screenShootRender, m_depthStencilView);

	UINT32 vertexSize = sizeof(PostAniDynamicVertex);
	UINT32 offset = 0;
	//_setAnimatedShaders();

	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/PixelShader.hlsl"), nullptr, 0);

	DirectX::BoundingFrustum * bf = _createBoundingFrustrum(&camera);
	for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
	{
		if (DX::g_animatedGeometryQueue[i]->getEntityType() != EntityType::FirstPersonPlayer)
			if (_Cull(bf, DX::g_animatedGeometryQueue[i]->getBoundingBox()))
				continue;

		if (DX::g_animatedGeometryQueue[i]->getHidden() != true)
		{
			//ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

			switch (camera.getPerspectiv())
			{
			case Camera::Perspectiv::Player:
				if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::PlayerType)
					continue;
				break;
			case Camera::Perspectiv::Enemy:
				if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::CurrentGuard || DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::FirstPersonPlayer)
					continue;
				break;
			}

			ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->GetAnimatedVertex();

			_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);

			if (DX::g_animatedGeometryQueue[i]->getTextureName().find("DOOR") <= DX::g_animatedGeometryQueue[i]->getTextureName().size())
			{
				int gfaghjk34h = 5;
			}

			DX::g_animatedGeometryQueue[i]->BindTextures();

			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			//_mapSkinningBuffer(DX::g_animatedGeometryQueue[i]);
			DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);

			//DX::g_animatedGeometryQueue[i]->TEMP();
		}
	}
	delete bf;
}

void ForwardRender::_InitScreenShoot()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = InputHandler::getWindowSize().x;
	textureDesc.Height = InputHandler::getWindowSize().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	
	if (SUCCEEDED(hr = DX::g_device->CreateTexture2D(&textureDesc, NULL, &m_screenShootTex)))
	{
		DX::SetName(m_screenShootTex, "m_screenShootTex");
	

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;


		if (SUCCEEDED(hr = DX::g_device->CreateRenderTargetView(m_screenShootTex, &renderTargetViewDesc, &m_screenShootRender)))
		{
			DX::SetName(m_screenShootRender, "m_screenShootRender");
		}
		

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		if (SUCCEEDED(hr = DX::g_device->CreateShaderResourceView(m_screenShootTex, &shaderResourceViewDesc, &m_screenShootSRV)))
		{
			DX::SetName(m_screenShootSRV, "m_screenShootSRV");
		}
	}
	
}
