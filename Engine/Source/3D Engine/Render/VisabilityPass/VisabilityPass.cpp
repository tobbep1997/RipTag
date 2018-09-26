#include "VisabilityPass.h"
#include "../../Extern.h"

VisabilityPass::VisabilityPass()
{
	_init();
}

VisabilityPass::~VisabilityPass()
{
	DX::SafeRelease(m_guardViewBuffer);
	DX::SafeRelease(m_guardDepthStencil);
	DX::SafeRelease(m_guardDepthTex);
	DX::SafeRelease(m_guardShaderResource);
}

void VisabilityPass::GuardDepthPrePassFor(Guard * guard)
{
	_mapViewBuffer(guard);

	// Static Objects
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(STATIC_VERTEX_SHADER_PATH));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(DEPTH_PRE_PASS_VERTEX_SHADER_PATH), nullptr,0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	
	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getEntityType() != EntityType::Player)
		{
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

			_mapObjectBuffer(DX::g_geometryQueue[i]);
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
		}
	}

	//TODO Dynamic Object

}

void VisabilityPass::CalculateVisabilityFor(Guard * guard)
{
	auto l_uav = guard->getUAV();


	DX::g_deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
		0,
		//&m_backBufferRTV,
		NULL,
		m_guardDepthStencil,
		//NULL,
		0, 1, &l_uav, 0
	);

	// Static Object
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(STATIC_VERTEX_SHADER_PATH));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(STATIC_VERTEX_SHADER_PATH), nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(VISABILITY_PASS_PIXEL_SHADER_PATH), nullptr, 0);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;

	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		if (DX::g_geometryQueue[i]->getEntityType() == EntityType::Player)
		{
			ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();

			_mapObjectBuffer(DX::g_geometryQueue[i]);
			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->getVertexSize(), 0);
		}

	}
	//TODO Dynamic Object


	guard->calcVisability();
}

void VisabilityPass::_init()
{
	_initViewPort();
	_initViewBuffer();
	_initObjectBuffer();
	_initDSV();
	_initSRV();
}

void VisabilityPass::_initViewPort()
{
	m_guardViewPort.Width = static_cast<float>(GUARD_RES);
	m_guardViewPort.Height = static_cast<float>(GUARD_RES);
	m_guardViewPort.MinDepth = 0.0f;
	m_guardViewPort.MaxDepth = 1.0f;
	m_guardViewPort.TopLeftX = 0;
	m_guardViewPort.TopLeftY = 0;
}

void VisabilityPass::_initViewBuffer()
{
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(GuardViewBuffer);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	HRESULT hr;
	hr = DX::g_device->CreateBuffer(&cameraBufferDesc, nullptr, &m_guardViewBuffer);
}

void VisabilityPass::_initObjectBuffer()
{
	D3D11_BUFFER_DESC objectBufferDesc;
	objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objectBufferDesc.ByteWidth = sizeof(ObjectBuffer);
	objectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	objectBufferDesc.MiscFlags = 0;
	objectBufferDesc.StructureByteStride = 0;
	HRESULT hr;
	hr = DX::g_device->CreateBuffer(&objectBufferDesc, nullptr, &m_objectBuffer);
}

void VisabilityPass::_initDSV()
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = GUARD_RES;
	depthStencilDesc.Height = GUARD_RES;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.FirstArraySlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Texture2D.MipSlice = 0;

	HRESULT hr;
	hr = DX::g_device->CreateTexture2D(&depthStencilDesc, NULL, &m_guardDepthTex);
	hr = DX::g_device->CreateDepthStencilView(m_guardDepthTex, &dsvDesc, &m_guardDepthStencil);
}

void VisabilityPass::_initSRV()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.ArraySize = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	HRESULT hr;
	hr = DX::g_device->CreateShaderResourceView(m_guardDepthTex, &srvDesc, &m_guardShaderResource);
}

void VisabilityPass::_initShaders()
{
	_initVertexShaders();
	_initPixelShaders();
}

void VisabilityPass::_initVertexShaders()
{
	DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DEPTH_PRE_PASS_VERTEX_SHADER_PATH);
}

void VisabilityPass::_initPixelShaders()
{
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(VISABILITY_PASS_PIXEL_SHADER_PATH);
}

void VisabilityPass::_mapViewBuffer(Guard * target)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	GuardViewBuffer gvb;
	gvb.cameraPosition = target->getCamera().getPosition();
	gvb.viewProjection = target->getCamera().getViewProjection();

	DX::g_deviceContext->Map(m_guardViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &gvb, sizeof(GuardViewBuffer));
	DX::g_deviceContext->Unmap(m_guardViewBuffer, 0);

	DX::g_deviceContext->VSSetConstantBuffers(1, 1, &m_guardViewBuffer);
	DX::g_deviceContext->PSSetConstantBuffers(2, 1, &m_guardViewBuffer);
}

void VisabilityPass::_mapObjectBuffer(Drawable * target)
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;

	ObjectBuffer ob;
	ob.worldMatrix = target->getWorldmatrix();

	DX::g_deviceContext->Map(m_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	memcpy(dataPtr.pData, &ob, sizeof(ObjectBuffer));
	DX::g_deviceContext->Unmap(m_objectBuffer, 0);

	DX::g_deviceContext->VSSetConstantBuffers(0, 1, &m_objectBuffer);
}
