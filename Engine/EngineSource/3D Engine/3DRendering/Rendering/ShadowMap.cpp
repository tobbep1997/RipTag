#include "EnginePCH.h"
#include "ShadowMap.h"


ShadowMap::ShadowMap()
{
}


ShadowMap::~ShadowMap()
{
}

void ShadowMap::Init(UINT width, UINT height)
{
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowMap/ShadowVertex.hlsl");
	ID3D11GeometryShader * test = DX::g_shaderManager.LoadShader<ID3D11GeometryShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowMap/ShadowGeometry.hlsl");

	_createRenderTargets(width, height);
	_createBuffers();
	_createShadowViewPort(width, height);
	_createShadowDepthStencilView(width, height);
}



void ShadowMap::ShadowPass(ForwardRender * renderingManager)
{
	this->Clear();
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowMap/ShadowVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(DX::g_shaderManager.GetShader<ID3D11GeometryShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowMap/ShadowGeometry.hlsl"), nullptr, 0);	
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_shadowViewport);
	m_runned = 0;

	//DirectX::BoundingSphere bs;
	for (unsigned int i = 0; i < DX::g_lights.size(); i++)
	{
		if (!DX::g_lights[i]->getUpdate())
			continue;
		DX::g_lights[i]->Clear();

		DX::g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, DX::g_lights[i]->getDSV());
		m_lightIndex.lightPos.x = i;
		for (int j = 0; j < 6; j++)
		{
			m_lightIndex.useSides[j].x = (UINT)DX::g_lights[i]->useSides()[j];
		}
		DXRHC::MapBuffer(m_lightIndexBuffer, &m_lightIndex, sizeof(LightIndex),13, 1, ShaderTypes::geometry);
		renderingManager->DrawInstanced(nullptr, &DX::INSTANCING::g_instanceShadowGroups);
	}

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/ShadowMap/ShadowVertexAnimated.hlsl"), nullptr, 0);

	UINT32 size = (UINT32)sizeof(PostAniDynamicVertex);
	UINT32 offset = 0U;
	for (unsigned int i = 0; i < DX::g_lights.size(); i++)
	{
		if (!DX::g_lights[i]->getUpdate())
			continue;

		DX::g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, DX::g_lights[i]->getDSV());
		m_lightIndex.lightPos.x = i;
		for (int j = 0; j < 6; j++)
		{
			m_lightIndex.useSides[j].x = (UINT)DX::g_lights[i]->useSides()[j];
		}
		DXRHC::MapBuffer(m_lightIndexBuffer, &m_lightIndex, sizeof(LightIndex), 13, 1, ShaderTypes::geometry);
		for (unsigned int j = 0; j < DX::g_animatedGeometryQueue.size(); j++)
		{
			if (DX::g_animatedGeometryQueue[j]->getHidden() != true)
			{
				ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[j]->GetAnimatedVertex();
				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &size, &offset);
				DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[j]->getVertexSize(), 0);
			}
		}
	}

	DX::g_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void ShadowMap::MapAllLightMatrix(std::vector<PointLight*> * lights)
{
	m_allLightMatrixValues.nrOfLights = DirectX::XMINT4(lights->size(), 0,0,0);
	for (unsigned int light = 0; light < lights->size(); light++)
	{
		m_allLightMatrixValues.nrOfviewProjection[light] = DirectX::XMINT4(lights->at(light)->getSides().size(),0,0,0);
		for (unsigned int i = 0; i < lights->at(light)->getSides().size(); i++)
		{
			m_allLightMatrixValues.viewProjection[light][i] = lights->at(light)->getSides().at(i)->getViewProjection();
			m_allLightMatrixValues.useDir[light][i].x = (UINT)lights->at(light)->useSides()[i];
		}
	}
	
	DXRHC::MapBuffer(m_allLightMatrixBuffer, &m_allLightMatrixValues, sizeof(PointLightBuffer));
	DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);
	DX::g_deviceContext->GSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);
	DX::g_deviceContext->PSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);

	
}

void ShadowMap::SetSamplerAndShaderResources()
{
	DX::g_deviceContext->PSSetSamplers(0, 1, &m_shadowSamplerState);
	for (int i = 0; i < DX::g_lights.size(); i++)
	{
		if (!DX::g_lights[i]->getUpdate())
			continue;
		DX::g_lights[i]->FirstRun();
		for (int j = 0; j < 6; j++)
		{
			if (DX::g_lights[i]->useSides()[j])
				DX::g_deviceContext->CopySubresourceRegion(m_shadowDepthBufferTex, (i * 6) + j, 0, 0, 0, DX::g_lights[i]->getTEX(), j, NULL);
				
		}
	}	

	DX::g_deviceContext->PSSetShaderResources(0, 1, &m_shadowShaderResourceView);
}

void ShadowMap::Clear()
{
	float c[4] = { 0.0f,0.0f,0.0f,1.0f };

	DX::g_deviceContext->ClearRenderTargetView(m_renderTargetView, c);
	//DX::g_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void ShadowMap::Release()
{
	DX::SafeRelease(m_shadowSamplerState);
	DX::SafeRelease(m_shadowDepthStencilView);
	DX::SafeRelease(m_shadowDepthBufferTex);
	for (int i = 0; i < 6; i++)
	{
	}
		DX::SafeRelease(m_shadowShaderResourceView);
	DX::SafeRelease(m_objectBuffer);
	
	DX::SafeRelease(m_allLightMatrixBuffer);

	DX::SafeRelease(m_renderTargetView);
	DX::SafeRelease(m_renderTargetsTexture);
	DX::SafeRelease(m_lightIndexBuffer);
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
	HRESULT hr;
	hr = DXRHC::CreateTexture2D(this->m_shadowDepthBufferTex, hight, width, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, SHADER_RESOURCE_VIEW_COUNT, 0, 0, DXGI_FORMAT_R32_TYPELESS);
	hr = DXRHC::CreateDepthStencilView(m_shadowDepthBufferTex, this->m_shadowDepthStencilView, 0, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, 0, SHADER_RESOURCE_VIEW_COUNT);
	hr = DXRHC::CreateShaderResourceView(m_shadowDepthBufferTex, m_shadowShaderResourceView, 0, DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, SHADER_RESOURCE_VIEW_COUNT, 0, 0, 1);
	hr = DXRHC::CreateSamplerState(m_shadowSamplerState, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_COMPARISON_LESS_EQUAL, 1.0f, 0.f);
}

void ShadowMap::_createBuffers()
{
	HRESULT hr = 0;
	hr = DXRHC::CreateConstantBuffer(m_objectBuffer, sizeof(ObjectBuffer));	
	hr = DXRHC::CreateConstantBuffer(m_allLightMatrixBuffer, sizeof(PointLightBuffer));
	hr = DXRHC::CreateConstantBuffer(m_lightIndexBuffer, sizeof(LightIndex));
}

void ShadowMap::_createRenderTargets(UINT width, UINT height)
{
	HRESULT hr;
	hr = DXRHC::CreateTexture2D(m_renderTargetsTexture, height, width, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, RENDER_TARGET_VIEW_COUNT, 0, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);
	hr = DXRHC::CreateRenderTargetView(m_renderTargetsTexture, m_renderTargetView, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, RENDER_TARGET_VIEW_COUNT);
}

void ShadowMap::_mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer)
{
	std::vector<DirectX::XMFLOAT4X4A> skinningVector = d->getAnimationPlayer()->GetSkinningMatrices();

	animBuffer->UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	animBuffer->SetToShader();
}

void ShadowMap::_mapObjectBuffer(Drawable * drawable)
{
	m_objectValues.worldMatrix = drawable->getWorldmatrix();
	DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex);
}
