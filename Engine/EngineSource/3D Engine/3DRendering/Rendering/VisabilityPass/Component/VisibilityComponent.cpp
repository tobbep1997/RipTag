#include "EnginePCH.h"
#include "VisibilityComponent.h"

VisibilityComponent::VisibilityComponent()
{
	m_playerVisability[0] = 0;
	m_playerVisability[1] = 0;
	m_pCam = nullptr;
}

VisibilityComponent::~VisibilityComponent()
{
	DX::SafeRelease(m_frustum.s_frustumBuffer);
	DX::SafeRelease(m_UAV.UAV);
	DX::SafeRelease(m_UAV.uavTextureBuffer);
	DX::SafeRelease(m_UAV.uavTextureBufferCPU);
}

void VisibilityComponent::Init(Camera * cam)
{
	m_pCam = cam;
	m_frustum.s_frustumData = NDCBOX::GetNDCBox();
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(FrustumVertex) * 36;
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_frustum.s_frustumData.data();

	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_frustum.s_frustumBuffer);

	_createUAV();
}

const std::vector<VisibilityComponent::FrustumVertex>* VisibilityComponent::getFrustum()
{
	return &m_frustum.s_frustumData;
}

ID3D11Buffer * VisibilityComponent::getFrustumBuffer()
{
	return m_frustum.s_frustumBuffer;
}

Camera * VisibilityComponent::getCamera()
{
	return m_pCam;
}

const int * VisibilityComponent::getVisibilityForPlayers() const
{
	return m_playerVisability;
}

void VisibilityComponent::QueueVisibility()
{
	DX::g_visibilityComponentQueue.push_back(this);
}

void VisibilityComponent::CalculateVisabilityFor(int playerIndex)
{
	DX::g_deviceContext->CopyResource(m_UAV.uavTextureBufferCPU, m_UAV.uavTextureBuffer);
	D3D11_MAPPED_SUBRESOURCE mr;

	struct ShadowTestData
	{
		unsigned int inside;
	};

	if (SUCCEEDED(DX::g_deviceContext->Map(m_UAV.uavTextureBufferCPU, 0, D3D11_MAP_READ, 0, &mr)))
	{
		ShadowTestData* data = (ShadowTestData*)mr.pData;

		m_playerVisability[playerIndex] = data->inside;
		DX::g_deviceContext->Unmap(m_UAV.uavTextureBufferCPU, 0);
	}
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	if (SUCCEEDED(DX::g_deviceContext->Map(m_UAV.uavTextureBufferCPU, 0, D3D11_MAP_WRITE, 0, &dataPtr)))
	{
		ShadowTestData killer = { 0 };
		memcpy(dataPtr.pData, &killer, sizeof(ShadowTestData));
		DX::g_deviceContext->CopyResource(m_UAV.uavTextureBuffer, m_UAV.uavTextureBufferCPU);
		//DX::g_deviceContext->CopyResource(m_uavTextureBuffer, m_uavTextureBufferCPU);
		DX::g_deviceContext->Unmap(m_UAV.uavTextureBufferCPU, 0);
	}
}

UINT32 VisibilityComponent::sizeOfFrustumVertex()
{
	return sizeof(FrustumVertex);
}

ID3D11UnorderedAccessView * VisibilityComponent::getUAV()
{
	return m_UAV.UAV;
}

void VisibilityComponent::Reset()
{
	m_playerVisability[0] = 0;
	m_playerVisability[1] = 0;
}

void VisibilityComponent::_createUAV()
{
	D3D11_TEXTURE2D_DESC TextureData;
	ZeroMemory(&TextureData, sizeof(TextureData));
	TextureData.ArraySize = 1;
	TextureData.Height = 1;
	TextureData.Width = 1;
	TextureData.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	TextureData.CPUAccessFlags = 0;
	TextureData.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	TextureData.MipLevels = 1;
	TextureData.MiscFlags = 0;
	TextureData.SampleDesc.Count = 1;
	TextureData.SampleDesc.Quality = 0;
	TextureData.Usage = D3D11_USAGE_DEFAULT;

	HRESULT hr;
	hr = DX::g_device->CreateTexture2D(&TextureData, NULL, &m_UAV.uavTextureBuffer);

	TextureData.Usage = D3D11_USAGE_STAGING;
	TextureData.BindFlags = 0;
	TextureData.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	hr = DX::g_device->CreateTexture2D(&TextureData, 0, &m_UAV.uavTextureBufferCPU);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	ZeroMemory(&UAVdesc, sizeof(UAVdesc));
	UAVdesc.Format = DXGI_FORMAT_R32_UINT;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Texture2D.MipSlice = 0;
	hr = DX::g_device->CreateUnorderedAccessView(m_UAV.uavTextureBuffer, &UAVdesc, &m_UAV.UAV);
}
