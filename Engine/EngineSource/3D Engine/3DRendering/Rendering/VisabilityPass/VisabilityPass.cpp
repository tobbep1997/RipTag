#include "EnginePCH.h"
#include "VisabilityPass.h"




VisabilityPass::VisabilityPass()
{
	m_animationBuffer = new Animation::AnimationCBuffer();
	m_animationBuffer->SetAnimationCBuffer();
}

VisabilityPass::~VisabilityPass()
{
	delete m_animationBuffer;
}

HRESULT VisabilityPass::Init(ForwardRender* forwardRender)
{
	p_forwardRender = forwardRender;
	return _init();
}

HRESULT VisabilityPass::Release()
{
	for (unsigned short int i = 0; i < MAX_GUARDS; i++)
	{
		DX::SafeRelease(m_guardViewBuffer[i]);
		DX::SafeRelease(m_guardDepthStencil[i]);
		DX::SafeRelease(m_guardDepthTex[i]);
		DX::SafeRelease(m_guardShaderResource[i]);

		DX::SafeRelease(m_guatdShaderResourceTex[i]);
		DX::SafeRelease(m_guardRenderTargetView[i]);
	}

		DX::SafeRelease(m_objectBuffer);

	DX::SafeRelease(m_textureBuffer);
	DX::SafeRelease(m_alphaBlend);
	return S_OK;
}

HRESULT VisabilityPass::Draw()
{
	size_t visSize = DX::g_visibilityComponentQueue.size();
	VisibilityComponent * current;


	DX::g_deviceContext->RSSetViewports(1, &m_guardViewPort);


	for (int i = 0; i < visSize && i < MAX_GUARDS; i++)
	{
		SetViewportAndRenderTarget(i);
		current = DX::g_visibilityComponentQueue[i];
		GuardDepthPrePassFor(current, p_forwardRender, m_animationBuffer);

		current->Reset();
		ID3D11UnorderedAccessView * l_uav = current->getUAV();
		DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
			1,
			&m_guardRenderTargetView[i],
			nullptr,
			1, 1, &l_uav, 0
		);

		CalculateVisabilityFor(i, current, m_animationBuffer);		
	}


	return S_OK;
}

HRESULT VisabilityPass::Draw(Drawable* drawable, const Camera& camera)
{
	return E_INVALIDARG;
}

HRESULT VisabilityPass::Draw(std::vector<DX::INSTANCING::GROUP> group, const Camera& camera)
{
	return E_INVALIDARG;
}

void VisabilityPass::GuardDepthPrePassFor(VisibilityComponent * target, ForwardRender * forwardRender, Animation::AnimationCBuffer * animBuffer)
{
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(STATIC_VERTEX_SHADER_PATH));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(DEPTH_PRE_PASS_STATIC_VERTEX_SHADER_PATH), nullptr,0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	

	DirectX::BoundingFrustum * boundingFrustum = forwardRender->_createBoundingFrustrum(target->getCamera());
	for (int i = 0; i < DX::g_cullQueue.size(); i++)
	{
		if (DX::g_cullQueue[i]->getEntityType() != EntityType::PlayerType && !DX::g_cullQueue[i]->getOutline())
		{
			if (DX::g_cullQueue[i]->getBoundingBox())
			{
				if (DX::g_cullQueue[i]->getBoundingBox()->Intersects(*boundingFrustum))
					DX::INSTANCING::tempInstance(DX::g_cullQueue[i]);
			}
			else
				DX::INSTANCING::tempInstance(DX::g_cullQueue[i]);

		}
	}
	delete boundingFrustum;

	forwardRender->DrawInstancedCull(target->getCamera());

	//forwardRender->_particlePass(target->getCamera(), true, PS::SMOKE);

	DX::g_deviceContext->OMSetBlendState(m_alphaBlend, 0, 0xffffffff); 

}

void VisabilityPass::CalculateVisabilityFor(const unsigned short int & index, VisibilityComponent * target, Animation::AnimationCBuffer * animBuffer)
{


	// Static Object
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(STATIC_VERTEX_SHADER_PATH));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(STATIC_VERTEX_SHADER_PATH), nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(VISABILITY_PASS_PIXEL_SHADER_PATH), nullptr, 0);

	DX::g_deviceContext->PSSetShaderResources(10, 1, &m_guardShaderResource[index]);

	_drawForPlayer(DX::g_player, target, 0);
	DX::g_deviceContext->Flush();
}

void VisabilityPass::SetViewportAndRenderTarget(const unsigned short int & index)
{
	float c[4] = { 0,0,0,0 };
	DX::g_deviceContext->OMSetRenderTargets(1, &m_guardRenderTargetView[index], m_guardDepthStencil[index]);

	DX::g_deviceContext->ClearRenderTargetView(m_guardRenderTargetView[index], c);
	DX::g_deviceContext->ClearDepthStencilView(m_guardDepthStencil[index], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_guardRenderTargetView[index], m_guardDepthStencil[index]);
}

HRESULT VisabilityPass::_init()
{
	HRESULT hr;
	_initViewPort();
	if (SUCCEEDED(hr = _initViewBuffer()))
	{		
		if (SUCCEEDED(_initObjectBuffer()))
		{			
			if(SUCCEEDED(_initDSV()))
			{
				if (SUCCEEDED(hr =DXRHC::CreateBlendState("VisabilityParticleBlendState", m_alphaBlend)))
				{
					for (int i = 0; i < MAX_GUARDS; i++)
					{
						if (SUCCEEDED(hr = DXRHC::CreateTexture2D("m_guatdShaderResourceTex",
							m_guatdShaderResourceTex[i],
							GUARD_RES_Y,
							GUARD_RES_X,
							D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
							1, 1, 0, 1, 0, 0,
							DXGI_FORMAT_R32G32B32A32_FLOAT,
							D3D11_USAGE_DEFAULT)))
						{
							if (SUCCEEDED(hr = DXRHC::CreateRenderTargetView("m_guardRenderTargetView", m_guatdShaderResourceTex[i], m_guardRenderTargetView[i], DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, 1)))
							{

							}
							else
								return hr;
						}
						else
							return hr;
					}


					if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("TextureBuffer", this->m_textureBuffer, sizeof(TextureBuffer))))
					{
						return S_OK;
					}
				}
			}
		}
	}
	return hr;
}

void VisabilityPass::_initViewPort()
{
	m_guardViewPort.Width = static_cast<float>(GUARD_RES_X);
	m_guardViewPort.Height = static_cast<float>(GUARD_RES_Y);
	m_guardViewPort.MinDepth = 0.0f;
	m_guardViewPort.MaxDepth = 1.0f;
	m_guardViewPort.TopLeftX = 0;
	m_guardViewPort.TopLeftY = 0;
}

HRESULT VisabilityPass::_initViewBuffer()
{
	HRESULT hr = 0;
	for (int i = 0; i < MAX_GUARDS; i++)
	{
		if (FAILED(hr = DXRHC::CreateConstantBuffer("GuardViewBuffer",this->m_guardViewBuffer[i], sizeof(GuardViewBuffer))))
		{
			return hr;
		}
	}
	return hr;
}

HRESULT VisabilityPass::_initObjectBuffer()
{
	HRESULT hr = 0;
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("ObjectBuffer", this->m_objectBuffer, sizeof(ObjectBuffer)))) { }
	return hr;

}

HRESULT VisabilityPass::_initDSV()
{
	HRESULT hr = 0;
	for (int i = 0; i < MAX_GUARDS; i++)
	{
		if (SUCCEEDED(hr = DXRHC::CreateTexture2D("m_guardDepthTex",
			m_guardDepthTex[i],
			GUARD_RES_Y,
			GUARD_RES_X,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
			1, 1, 0, 1, 0, 0,
			DXGI_FORMAT_R32_TYPELESS)))
		{
			if (SUCCEEDED(hr = DXRHC::CreateDepthStencilView("m_guardDepthStencil",
				m_guardDepthTex[i],
				m_guardDepthStencil[i],
				0,
				DXGI_FORMAT_D32_FLOAT,
				D3D11_DSV_DIMENSION_TEXTURE2D,
				0, 1)))
			{
				if (SUCCEEDED(hr = DXRHC::CreateShaderResourceView("m_guardShaderResource",
					m_guardDepthTex[i],
					m_guardShaderResource[i],
					0,
					DXGI_FORMAT_R32_FLOAT,
					D3D11_SRV_DIMENSION_TEXTURE2D,
					1, 0, 0, 1)))
				{
				}
				else
					return hr;
			}
			else
				return hr;
		}
		else
			return hr;
	}
	return hr;
}

HRESULT VisabilityPass::_initShaders()
{
	HRESULT hr = 0;
	if (SUCCEEDED(hr = _initVertexShaders()))
		if (SUCCEEDED(hr = _initPixelShaders()))
			return S_OK;
	return hr;
}

HRESULT VisabilityPass::_initVertexShaders()
{
	if (DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DEPTH_PRE_PASS_STATIC_VERTEX_SHADER_PATH))
		return S_OK;
	return E_INVALIDARG;
}

HRESULT VisabilityPass::_initPixelShaders()
{
	if (DX::g_shaderManager.LoadShader<ID3D11PixelShader>(VISABILITY_PASS_PIXEL_SHADER_PATH))
		return S_OK;
	return E_INVALIDARG;
}

HRESULT VisabilityPass::_mapViewBuffer(const unsigned short int & index, VisibilityComponent * target)
{
	HRESULT hr;
	GuardViewBuffer gvb;
	gvb.cameraPosition = target->getCamera()->getPosition();
	gvb.viewProjection = target->getCamera()->getViewProjection();


	if (FAILED(hr = DXRHC::MapBuffer(m_guardViewBuffer[index], &gvb, sizeof(GuardViewBuffer))))
	{
		return hr;
	}
	

	DX::g_deviceContext->VSSetConstantBuffers(2, 1, &m_guardViewBuffer[index]);
	DX::g_deviceContext->PSSetConstantBuffers(2, 1, &m_guardViewBuffer[index]);

	return S_OK;
}

void VisabilityPass::_mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer)
{
	std::vector<DirectX::XMFLOAT4X4A> skinningVector = d->getAnimationPlayer()->GetSkinningMatrices();

	animBuffer->UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	animBuffer->SetToShader();
}

HRESULT VisabilityPass::_mapObjectBuffer(Drawable * target)
{
	ObjectBuffer ob;
	ob.worldMatrix = target->getWorldmatrix();
	HRESULT hr;
	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_objectBuffer, &ob, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex))) {}

	m_textureValues.textureTileMult.x = target->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = target->getTextureTileMult().y;

	m_textureValues.usingTexture.x = target->isTextureAssigned();

	m_textureValues.color = target->getColor();

	if (SUCCEEDED(hr = DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel))){}
	return hr;
}

HRESULT VisabilityPass::_drawForPlayer(Drawable * player, VisibilityComponent * target, int playerIndex)
{
	UINT32 vertexSize = sizeof(PostAniDynamicVertex);
	UINT32 offset = 0;
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"));

	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/AnimatedVertexShader.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);

	ID3D11Buffer * vertexBuffer = DX::g_player->GetAnimatedVertex();
	_mapObjectBuffer(DX::g_player);

	DX::g_player->BindTextures();
	DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);

	DX::g_deviceContext->Draw(DX::g_player->getVertexSize(), 0);

	target->CalculateVisabilityFor(playerIndex);
	return S_OK;
}
