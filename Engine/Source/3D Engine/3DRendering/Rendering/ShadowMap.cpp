#include "ShadowMap.h"
#include "../Framework/DirectXRenderingHelpClass.h"

ShadowMap::ShadowMap()
{
}


ShadowMap::~ShadowMap()
{
}

void ShadowMap::Init(UINT width, UINT height)
{
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/ShadowVertex.hlsl");
	ID3D11GeometryShader * test = DX::g_shaderManager.LoadShader<ID3D11GeometryShader>(L"../Engine/Source/Shader/Shaders/ShadowGeometry.hlsl");

	_createRenderTargets(width, height);
	_createBuffers();
	_createShadowViewPort(width, height);
	_createShadowDepthStencilView(width, height);
}



void ShadowMap::ShadowPass(Animation::AnimationCBuffer * animBuffer)
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/ShadowVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(DX::g_shaderManager.GetShader<ID3D11GeometryShader>(L"../Engine/Source/Shader/Shaders/ShadowGeometry.hlsl"), nullptr, 0);	
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_shadowViewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_shadowDepthStencilView);

	for (unsigned int j = 0; j < DX::g_geometryQueue.size(); j++)
	{
		UINT32 vertexSize = sizeof(StaticVertex);
		UINT32 offset = 0;

		ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[j]->getBuffer();

		_mapObjectBuffer(DX::g_geometryQueue[j]);
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_geometryQueue[j]->getVertexSize(), 0);
	}
	if (animBuffer && !DX::g_animatedGeometryQueue.empty())
	{
		UINT32 vertexSize = sizeof(DynamicVertex);
		UINT32 offset = 0;
		DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl"));
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/Source/Shader/Shaders/ShadowVertexAnimated.hlsl"), nullptr, 0);
		for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
		{
			ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();

			_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);

			//DX::g_animatedGeometryQueue[i]->BindTextures();

			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			_mapSkinningBuffer(DX::g_animatedGeometryQueue[i], animBuffer);
			DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
		}
	}
	DX::g_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void ShadowMap::MapAllLightMatrix(std::vector<PointLight*> * lights)
{
	m_allLightMatrixValues.nrOfLights = DirectX::XMINT4(lights->size(),0,0,0);
	for (unsigned int light = 0; light < lights->size(); light++)
	{
		m_allLightMatrixValues.nrOfviewProjection[light] = DirectX::XMINT4(lights->at(light)->getSides().size(),0,0,0);
		for (unsigned int i = 0; i < lights->at(light)->getSides().size(); i++)
		{
			m_allLightMatrixValues.viewProjection[light][i] = lights->at(light)->getSides()[i]->getViewProjection();
		}
	}
	
	DXRHC::MapBuffer(m_allLightMatrixBuffer, &m_allLightMatrixValues, sizeof(PointLightBuffer));
	//DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);
	DX::g_deviceContext->GSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);
	DX::g_deviceContext->PSSetConstantBuffers(1, 1, &m_allLightMatrixBuffer);
}

void ShadowMap::SetSamplerAndShaderResources()
{
	DX::g_deviceContext->PSSetSamplers(0, 1, &m_shadowSamplerState);
	DX::g_deviceContext->PSSetShaderResources(0, 1, &m_shadowShaderResourceView);	
}

void ShadowMap::Clear()
{
	float c[4] = { 0.0f,0.0f,0.0f,1.0f };

	DX::g_deviceContext->ClearRenderTargetView(m_renderTargetView, c);
	DX::g_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
	hr = DXRHC::CreateTexture2D(this->m_shadowDepthBufferTex, hight, width, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, RENDER_TARGET_VIEW_COUNT, 0, 0, DXGI_FORMAT_R32_TYPELESS);
	hr = DXRHC::CreateDepthStencilView(m_shadowDepthBufferTex, this->m_shadowDepthStencilView, 0, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, 0, RENDER_TARGET_VIEW_COUNT);	
	hr = DXRHC::CreateShaderResourceView(m_shadowDepthBufferTex, m_shadowShaderResourceView, 0, DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, RENDER_TARGET_VIEW_COUNT, 0, 0, 1);
	hr = DXRHC::CreateSamplerState(m_shadowSamplerState, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D11_COMPARISON_LESS_EQUAL, 1.0f, 0.f);
}

void ShadowMap::_createBuffers()
{
	HRESULT hr = 0;
	hr = DXRHC::CreateConstantBuffer(m_objectBuffer, sizeof(ObjectBuffer));	
	hr = DXRHC::CreateConstantBuffer(m_allLightMatrixBuffer, sizeof(PointLightBuffer));
}

void ShadowMap::_createRenderTargets(UINT width, UINT height)
{
	HRESULT hr;
	hr = DXRHC::CreateTexture2D(m_renderTargetsTexture, height, width, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, RENDER_TARGET_VIEW_COUNT, 0, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);
	hr = DXRHC::CreateRenderTargetView(m_renderTargetsTexture, m_renderTargetView, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, RENDER_TARGET_VIEW_COUNT);
}

void ShadowMap::_mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer)
{
	std::vector<DirectX::XMFLOAT4X4A> skinningVector = d->getAnimatedModel()->GetSkinningMatrices();

	animBuffer->UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	animBuffer->SetToShader();
}

void ShadowMap::_mapObjectBuffer(Drawable * drawable)
{
	m_objectValues.worldMatrix = drawable->getWorldmatrix();
	DXRHC::MapBuffer(m_objectBuffer, &m_objectValues, sizeof(ObjectBuffer), 0, 1, ShaderTypes::vertex);
}
