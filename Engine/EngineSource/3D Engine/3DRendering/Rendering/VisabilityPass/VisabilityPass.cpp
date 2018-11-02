#include "VisabilityPass.h"
#include "../../../Extern.h"
#include "../../Framework/DirectXRenderingHelpClass.h"

VisabilityPass::VisabilityPass()
{
	
}

VisabilityPass::~VisabilityPass()
{
	DX::SafeRelease(m_guardViewBuffer);
	DX::SafeRelease(m_guardDepthStencil);
	DX::SafeRelease(m_guardDepthTex);
	DX::SafeRelease(m_guardShaderResource);
	DX::SafeRelease(m_objectBuffer);

	DX::SafeRelease(m_guatdShaderResourceTex);
	DX::SafeRelease(m_guardRenderTargetView);

	DX::SafeRelease(m_textureBuffer);
}

void VisabilityPass::Init()
{
	_init();
}

void VisabilityPass::GuardDepthPrePassFor(VisibilityComponent * target, Animation::AnimationCBuffer * animBuffer)
{
	_mapViewBuffer(target); 
	ID3D11ShaderResourceView * tes = nullptr;
	DX::g_deviceContext->PSSetShaderResources(10, 1, &tes);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_guardRenderTargetView, m_guardDepthStencil);
	
	float c[4] = { 0,0,0,0 };
	DX::g_deviceContext->ClearRenderTargetView(m_guardRenderTargetView, c);
	DX::g_deviceContext->ClearDepthStencilView(m_guardDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
	//	0,
	//	nullptr,
	//	nullptr,
	//	//NULL,
	//	0, 0, nullptr, 0
	//);
	//DX::g_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	
	// Static Objects
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(STATIC_VERTEX_SHADER_PATH));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(DEPTH_PRE_PASS_STATIC_VERTEX_SHADER_PATH), nullptr,0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getEntityType() != EntityType::PlayerType && DX::g_geometryQueue[i]->getEntityType() != EntityType::ExcludeType)
		{
			_mapObjectBuffer(DX::g_geometryQueue[i]);
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
		}
	}
	if (animBuffer && !DX::g_animatedGeometryQueue.empty())
	{
		UINT32 vertexSize = sizeof(DynamicVertex);
		UINT32 offset = 0;
		DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DYNAMIC_VERTEX_SHADER_PATH));
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DEPTH_PRE_PASS_DYNAMIC_VERTEX_SHADER_PATH), nullptr, 0);
		for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
		{
			if (DX::g_animatedGeometryQueue[i]->getEntityType() != EntityType::PlayerType && DX::g_animatedGeometryQueue[i]->getEntityType() != EntityType::ExcludeType)
			{
				ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();
				_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);
				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
				_mapSkinningBuffer(DX::g_animatedGeometryQueue[i], animBuffer);
				DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
			}

		}
	}
}

void VisabilityPass::CalculateVisabilityFor(VisibilityComponent * target, Animation::AnimationCBuffer * animBuffer)
{
	target->Reset();
	ID3D11UnorderedAccessView * l_uav = target->getUAV();
	DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
		1,
		&m_guardRenderTargetView,
		nullptr,
		//NULL,
		1, 1, &l_uav, 0
	);

	// Static Object
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(STATIC_VERTEX_SHADER_PATH));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(STATIC_VERTEX_SHADER_PATH), nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(VISABILITY_PASS_PIXEL_SHADER_PATH), nullptr, 0);

	DX::g_deviceContext->PSSetShaderResources(10, 1, &m_guardShaderResource);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;

	int playerIndex = 0;

	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getEntityType() == EntityType::PlayerType)
		{
			_mapObjectBuffer(DX::g_geometryQueue[i]);
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();
			DX::g_geometryQueue[i]->BindTextures();
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
			target->CalculateVisabilityFor(playerIndex++);
		}

	}
	if (animBuffer && !DX::g_animatedGeometryQueue.empty())
	{
		UINT32 vertexSize = sizeof(DynamicVertex);
		UINT32 offset = 0;
		DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(DYNAMIC_VERTEX_SHADER_PATH));
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DYNAMIC_VERTEX_SHADER_PATH), nullptr, 0);
		for (unsigned int i = 0; i < DX::g_animatedGeometryQueue.size(); i++)
		{
			if (DX::g_animatedGeometryQueue[i]->getEntityType() == EntityType::PlayerType)
			{
				ID3D11Buffer * vertexBuffer = DX::g_animatedGeometryQueue[i]->getBuffer();
				_mapObjectBuffer(DX::g_animatedGeometryQueue[i]);
				DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
				_mapSkinningBuffer(DX::g_animatedGeometryQueue[i], animBuffer);
				DX::g_deviceContext->Draw(DX::g_animatedGeometryQueue[i]->getVertexSize(), 0);
				target->CalculateVisabilityFor(playerIndex++);
			}

		}
	}
	//DX::g_deviceContext->PSSetShaderResources(10, 0, nullptr);
}

void VisabilityPass::SetViewportAndRenderTarget()
{
	DX::g_deviceContext->RSSetViewports(1, &m_guardViewPort);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_guardRenderTargetView, m_guardDepthStencil);
}

void VisabilityPass::_init()
{
	_initViewPort();
	_initViewBuffer();
	_initObjectBuffer();
	_initDSV();
	_initSRV();

	HRESULT hr;
	hr = DXRHC::CreateTexture2D(m_guatdShaderResourceTex, GUARD_RES_Y, GUARD_RES_X, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, 1, 0, 0, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT);
	hr = DXRHC::CreateRenderTargetView(m_guatdShaderResourceTex, m_guardRenderTargetView, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, 1);
	hr = DXRHC::CreateConstantBuffer(this->m_textureBuffer, sizeof(TextureBuffer));
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

void VisabilityPass::_initViewBuffer()
{
	HRESULT hr;
	hr = DXRHC::CreateConstantBuffer(this->m_guardViewBuffer, sizeof(GuardViewBuffer));
}

void VisabilityPass::_initObjectBuffer()
{

	HRESULT hr = DXRHC::CreateConstantBuffer(this->m_objectBuffer, sizeof(ObjectBuffer));

}

void VisabilityPass::_initDSV()
{
	HRESULT hr;
	hr = DXRHC::CreateTexture2D(m_guardDepthTex, GUARD_RES_Y, GUARD_RES_X, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, 1, 0, 0, DXGI_FORMAT_R32_TYPELESS);
	hr = DXRHC::CreateDepthStencilView(m_guardDepthTex, m_guardDepthStencil, 0, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2D, 0, 1);
	hr = DXRHC::CreateShaderResourceView(m_guardDepthTex, m_guardShaderResource, 0, DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2D, 1, 0, 0, 1);
}

void VisabilityPass::_initSRV()
{	
}

void VisabilityPass::_initShaders()
{
	_initVertexShaders();
	_initPixelShaders();
}

void VisabilityPass::_initVertexShaders()
{
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DEPTH_PRE_PASS_STATIC_VERTEX_SHADER_PATH);
}

void VisabilityPass::_initPixelShaders()
{
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(VISABILITY_PASS_PIXEL_SHADER_PATH);
}

void VisabilityPass::_mapViewBuffer(VisibilityComponent * target)
{
	
	GuardViewBuffer gvb;
	gvb.cameraPosition = target->getCamera()->getPosition();
	gvb.viewProjection = target->getCamera()->getViewProjection();

	DXRHC::MapBuffer(m_guardViewBuffer, &gvb, sizeof(GuardViewBuffer));

	DX::g_deviceContext->VSSetConstantBuffers(2, 1, &m_guardViewBuffer);
	DX::g_deviceContext->PSSetConstantBuffers(2, 1, &m_guardViewBuffer);
}

void VisabilityPass::_mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer)
{
	std::vector<DirectX::XMFLOAT4X4A> skinningVector = d->getAnimatedModel()->GetSkinningMatrices();

	animBuffer->UpdateBuffer(skinningVector.data(), skinningVector.size() * sizeof(float) * 16);
	animBuffer->SetToShader();
}

void VisabilityPass::_mapObjectBuffer(Drawable * target)
{
	ObjectBuffer ob;
	ob.worldMatrix = target->getWorldmatrix();

	DXRHC::MapBuffer(m_objectBuffer, &ob, sizeof(ObjectBuffer), 3, 1, ShaderTypes::vertex);

	m_textureValues.textureTileMult.x = target->getTextureTileMult().x;
	m_textureValues.textureTileMult.y = target->getTextureTileMult().y;

	m_textureValues.usingTexture.x = target->isTextureAssigned();

	m_textureValues.color = target->getColor();

	DXRHC::MapBuffer(m_textureBuffer, &m_textureValues, sizeof(TextureBuffer), 7, 1, ShaderTypes::pixel);
}
