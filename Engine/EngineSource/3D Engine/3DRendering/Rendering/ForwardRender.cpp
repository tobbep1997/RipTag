#include "EnginePCH.h"
#include "ForwardRender.h"

ForwardRender::ForwardRender()
{
	m_lastVertexPath = L"NULL";
	m_lastPixelPath = L"NULL";

	m_visabilityPass = new VisabilityPass();

}


ForwardRender::~ForwardRender()
{
	delete m_visabilityPass;
}

void ForwardRender::Init(IDXGISwapChain * swapChain, ID3D11RenderTargetView * backBufferRTV, ID3D11DepthStencilView * depthStencilView, ID3D11DepthStencilState* m_depthStencilState, ID3D11Texture2D * depthBufferTex, ID3D11SamplerState * samplerState, D3D11_VIEWPORT viewport)
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


	_createConstantBuffer();
	_createSamplerState();

	_OutlineDepthCreate();
	m_shadowMap = new ShadowMap();
	m_shadowMap->Init(128, 128);
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
	HRESULT hr = DX::g_device->CreateBlendState(&omDesc, &m_alphaBlend);

	
	m_visabilityPass->Init();


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

	m_animationBuffer = new Animation::AnimationCBuffer();
	m_animationBuffer->SetAnimationCBuffer();

	m_2DRender = new Render2D();
	m_2DRender->Init();

}

void ForwardRender::GeometryPass()
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff);
	
	
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	_setStaticShaders();
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getHidden() != true) //&& DX::g_geometryQueue[i]->getOutline() != true)
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

void ForwardRender::PrePass(Camera & camera)
{
	if (m_firstRun == true)
	{
		m_shaderThreads[0].join();
		//m_shaderThreads[1].join();
		m_shaderThreads[2].join();
		m_firstRun = false;
	}
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
	//DX::g_deviceContext->ClearDepthStencilView(m_outlineDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	//_setStaticShaders();
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getHidden() != true && DX::g_geometryQueue[i]->GetTransparant() != true)
		{
			if (DX::g_geometryQueue[i]->getOutline() == true)
			{
				//DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_outlineDepthStencil);
				{
					DX::g_deviceContext->OMSetDepthStencilState(m_write0State, 0);
					ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

					_mapObjectOutlineBuffer(DX::g_geometryQueue[i], camera.getPosition());
					DX::g_geometryQueue[i]->BindTextures();
					DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
					DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
				}
				{
					DX::g_deviceContext->OMSetDepthStencilState(m_write1State, 0);
					ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

					_mapObjectInsideOutlineBuffer(DX::g_geometryQueue[i], camera.getPosition());
					DX::g_geometryQueue[i]->BindTextures();
					DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
					DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
				}
				DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
				//DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
			}
			else
			{
				ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

				_mapObjectBuffer(DX::g_geometryQueue[i]);
				DX::g_geometryQueue[i]->BindTextures();
				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
				DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
			}
			
		}
	}

	DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);
}

void ForwardRender::AnimatedGeometryPass(Camera & camera)
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
		if (DX::g_animatedGeometryQueue[i]->getHidden() != true )
		{
			if (DX::g_animatedGeometryQueue[i]->getOutline() == true)
			{
				/*{
					DX::g_deviceContext->OMSetDepthStencilState(m_write0State, 0);
					ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

					_mapObjectOutlineBuffer(DX::g_animatedGeometryQueue[i], camera.getPosition());
					DX::g_animatedGeometryQueue[i]->BindTextures();
					DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
					DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
				}
				{
					DX::g_deviceContext->OMSetDepthStencilState(m_write1State, 0);
					ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

					_mapObjectInsideOutlineBuffer(DX::g_animatedGeometryQueue[i], camera.getPosition());
					DX::g_animatedGeometryQueue[i]->BindTextures();
					DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
					DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
				}
				DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);*/
			}
			else
			{
				//todoREMOVE
				auto animatedModel = DX::g_animatedGeometryQueue[i]->getAnimatedModel();

				ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

				_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);

				DX::g_animatedGeometryQueue[i]->BindTextures();

				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
				_mapSkinningBuffer(DX::g_animatedGeometryQueue[i]);
				DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
			}
			
			
		}		
	}
}

void ForwardRender::Flush(Camera & camera)
{
	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, NULL);
	_mapCameraBuffer(camera);
	this->PrePass(camera);


	DX::g_deviceContext->PSSetSamplers(1, 1, &m_samplerState);
	DX::g_deviceContext->PSSetSamplers(2, 1, &m_shadowSampler);
	_simpleLightCulling(camera);
	this->m_shadowMap->MapAllLightMatrix(&DX::g_lights);
	_mapLightInfoNoMatrix();

	this->m_shadowMap->ShadowPass(m_animationBuffer);

	this->m_shadowMap->SetSamplerAndShaderResources();
	_visabilityPass();
	_mapCameraBuffer(camera);
	this->GeometryPass();
	this->AnimatedGeometryPass(camera);
	this->_OutliningPass(camera);


	//_GuardFrustumDraw();
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	m_2DRender->GUIPass();
	this->_wireFramePass();
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
	//this->m_shadowMap->Clear();
	DX::g_visibilityComponentQueue.clear();

	DX::g_wireFrameDrawQueue.clear();
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

	DX::SafeRelease(m_outlineBuffer);
	m_shadowMap->Release();
	delete m_shadowMap;

	m_2DRender->Release();
	delete m_2DRender;
	delete m_animationBuffer;
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


}

void ForwardRender::_GuardLightCulling()
{
	//Get all the guards for this frame
	std::vector<Drawable*> guards;
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); ++i)
	{
		if (DX::g_geometryQueue.at(i)->getEntityType() == EntityType::GuarddType)
		{
			guards.push_back(DX::g_geometryQueue.at(i));
		}
	}

	std::vector<PointLight*> lights;
	std::vector<int> indexs;

	for (unsigned int i = 0; i < guards.size(); ++i)
	{
		float bobbyDickLenght = 1000000;
		int lightIndex = -1;
		for (unsigned int j = 0; j < DX::g_lights.size(); ++j)
		{
			float lenght = DX::g_lights.at(j)->getDistanceFromObject(guards.at(i)->getPosition());
			if (lenght < bobbyDickLenght)
			{
				bobbyDickLenght = lenght;
				lightIndex = j;
			}
		}
		if (lightIndex != -1)
		{
			lights.push_back(DX::g_lights.at(lightIndex));
			indexs.push_back(lightIndex);
		}
	}
	//bool Culled = false;
	//while (Culled == false)
	//{
	//	bool ff = false;
	//	for (unsigned int i = 0; i < DX::g_lights.size(); ++i)
	//	{
	//		bool found = false;
	//		for (unsigned int j = 0; j < lights.size(); ++j)
	//		{
	//			if (DX::g_lights.at(i) != lights.at(j))
	//			{
	//				found = true;
	//				DX::g_lights.erase(DX::g_lights.begin() + i);
	//				//lights.erase(lights.begin() + j);
	//				break;
	//			}
	//		}
	//		if (found == true)
	//		{
	//			ff = true;
	//			break;
	//		}
	//		
	//	}
	//	if (ff == false)
	//	{
	//		Culled = true;
	//	}
	//	
	//}

	DX::g_lights.clear();
	DX::g_lights = lights;
	
	ImGui::Begin("lgihts");
	ImGui::Text("Lights, %d", DX::g_lights.size());
	ImGui::End();
}

void ForwardRender::_createConstantBuffer()
{
	HRESULT hr;
	hr = DXRHC::CreateConstantBuffer(this->m_objectBuffer, sizeof(ObjectBuffer));	
	hr = DXRHC::CreateConstantBuffer(this->m_cameraBuffer, sizeof(CameraBuffer));
	hr = DXRHC::CreateConstantBuffer(this->m_lightBuffer, sizeof(LightBuffer));
	hr = DXRHC::CreateConstantBuffer(this->m_GuardBuffer, sizeof(GuardBuffer));
	hr = DXRHC::CreateConstantBuffer(this->m_textureBuffer, sizeof(TextureBuffer));

	hr = DXRHC::CreateConstantBuffer(this->m_outlineBuffer, sizeof(OutLineBuffer));
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

void ForwardRender::_mapObjectOutlineBuffer(Drawable* drawable, const DirectX::XMFLOAT4A & pos)
{
	m_objectValues.worldMatrix = drawable->getWorldMatrixForOutline(pos);
	DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex);



	m_textureValues.textureTileMult.x = drawable->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = drawable->getTextureTileMult().y;

	m_textureValues.usingTexture.x = drawable->isTextureAssigned();

	m_textureValues.color = DirectX::XMFLOAT4A(2000,0,0,1);

	DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel);
}

void ForwardRender::_mapObjectInsideOutlineBuffer(Drawable* drawable, const DirectX::XMFLOAT4A& pos)
{
	m_objectValues.worldMatrix = drawable->getWorldMatrixForInsideOutline(pos);
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

	m_animationBuffer->UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	m_animationBuffer->SetToShader();
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

void ForwardRender::_OutliningPass(Camera & cam)
{

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	//DX::g_deviceContext->OMSetDepthStencilState(depthoutState, 0);
	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	_setStaticShaders();
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getOutline() == true)
		{
			{
				m_outLineValues.outLineColor = DX::g_geometryQueue[i]->getOutlineColor();
				//DX::g_deviceContext->PSSetShaderResources(10, 1, &m_outlineShaderRes);

				DXRHC::MapBuffer(m_outlineBuffer, &m_outLineValues, sizeof(OutLineBuffer), 8, 1, ShaderTypes::pixel);

				DX::g_deviceContext->OMSetDepthStencilState(m_OutlineState, 0);

				DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/OutlineVertexShader.hlsl"), nullptr, 0);
				DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePixelShader.hlsl"), nullptr, 0);
				ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

				_mapObjectOutlineBuffer(DX::g_geometryQueue[i], cam.getPosition());
				DX::g_geometryQueue[i]->BindTextures();
				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
				DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
			}
			
		}
	}

	//_setAnimatedShaders();
	//DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePixelShader.hlsl"), nullptr, 0);
	//for (int i = 0; i < DX::g_animatedGeometryQueue.size(); ++i)
	//{
	//	if (DX::g_animatedGeometryQueue[i]->getOutline() == true)
	//	{
	//		m_outLineValues.outLineColor = DX::g_animatedGeometryQueue[i]->getOutlineColor();
	//		//DX::g_deviceContext->PSSetShaderResources(10, 1, &m_outlineShaderRes);

	//		DXRHC::MapBuffer(m_outlineBuffer, &m_outLineValues, sizeof(OutLineBuffer), 8, 1, ShaderTypes::pixel);

	//		DX::g_deviceContext->OMSetDepthStencilState(m_OutlineState, 0);

	//		//DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/OutlineVertexShader.hlsl"), nullptr, 0);
	//		//DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePixelShader.hlsl"), nullptr, 0);
	//		ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

	//		_mapObjectOutlineBuffer(DX::g_animatedGeometryQueue[i], cam.getPosition());
	//		DX::g_animatedGeometryQueue[i]->BindTextures();
	//		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	//		DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
	//	}
	//}

}

void ForwardRender::_OutlineDepthCreate()
{
//;
//	
//	HRESULT hr = DX::g_device->CreateTexture2D(&depthStencilDesc, NULL, &m_outlineDepthBufferTex);
//	hr = DX::g_device->CreateDepthStencilView(m_outlineDepthBufferTex, NULL, &m_outlineDepthStencil);

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

	DX::g_device->CreateDepthStencilState(&depth, &m_write1State);

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

	DX::g_device->CreateDepthStencilState(&depth2, &m_write0State);

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

	DX::g_device->CreateDepthStencilState(&depth2, &m_OutlineState);



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

void ForwardRender::_visabilityPass()
{
	m_visabilityPass->SetViewportAndRenderTarget();
	for (VisibilityComponent * guard : DX::g_visibilityComponentQueue)
	{
		m_visabilityPass->GuardDepthPrePassFor(guard, m_animationBuffer);
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
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/OutlinePixelShader.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertex.hlsl");
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/VisabilityShader/PreDepthPassVertexAnimated.hlsl");

	
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
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/wireFramePixel.hlsl"), nullptr, 0);
	for (unsigned int i = 0; i < DX::g_wireFrameDrawQueue.size(); i++)
	{
		ID3D11Buffer * vertexBuffer = DX::g_wireFrameDrawQueue[i]->getBuffer();

		_mapObjectBuffer(DX::g_wireFrameDrawQueue[i]);
		DX::g_wireFrameDrawQueue[i]->BindTextures();
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_wireFrameDrawQueue[i]->getVertexSize(), 0);

	}

	//DX::g_deviceContext->RSSetState(m_standardRast);
}


