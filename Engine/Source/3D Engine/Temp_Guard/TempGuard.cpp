#include "TempGuard.h"
#include "../../../../RipTag/Source/Game/States/PlayState.h"
//#include "../../Debugg/ImGui/imgui.h"
Guard::Guard()
{
	m_frustum.setPoints();
	m_pos = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
	m_rot = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
	//m_cam.setDirection(0.0f, 0.0f, -1.0f);
	m_cam = Camera(DirectX::XMConvertToRadians(150.0f / 2.0f), 250.0f / 150.0f);
	m_cam.setUP(0, 1, 0);
	m_cam.setPosition(0.0f, 1.0f, 0.0f);
	m_cam.setNearPlane(0.1f);
	m_cam.setFarPlane(20.0f);
	m_cam.setDirection(0, 0.5, 0.5);
	m_range = 20.0f;


	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(FrustumVertex) * 36;
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = &m_frustum.p;

	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer);

	_createUAV();
}

Guard::~Guard()
{
	DX::SafeRelease(m_vertexBuffer);

	DX::SafeRelease(m_uavTextureBuffer);
	DX::SafeRelease(m_uavTextureBufferCPU);
	DX::SafeRelease(m_visabilityUAV);
	
}

const Frustum & Guard::getFrustum()
{
	return m_frustum;
}

void Guard::setPos(float x, float y, float z)
{

	m_cam.setPosition(x, y + 1, z);
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

DirectX::XMFLOAT4A Guard::getPos()
{
	return m_pos;
}

void Guard::setDir(float x, float y, float z)
{
	m_cam.setDirection(x, y, z);
	m_rot.x = x;
	m_rot.y = y;
	m_rot.z = z;
}

DirectX::XMFLOAT4A Guard::getDir()
{
	return m_rot;
}

void Guard::Rotate(float x, float y, float z)
{
	m_cam.Rotate(x, y, z);
}

ID3D11Buffer* Guard::getVertexBuffer()
{
	return m_vertexBuffer;
}

UINT32 Guard::getSizeOfStruct()
{
	return sizeof(FrustumVertex);
}

UINT Guard::getNrVertices()
{
	return Frustum::NR_OF_VERTICES;
}

Camera & Guard::getCamera()
{
	return m_cam;
}



const DirectX::XMFLOAT4X4A & Guard::getWorldMatrix()
{
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation
	(
		this->m_pos.x,
		this->m_pos.y,
		this->m_pos.z
	);
	translation = DirectX::XMMatrixTranspose(translation);
	DirectX::XMStoreFloat4x4A(&m_worldMatrix, translation);

	return m_worldMatrix;
}

void Guard::Draw()
{
	DX::g_guardDrawQueue.push_back(this);
}

void Guard::calcVisability()
{
	DX::g_deviceContext->CopyResource(m_uavTextureBufferCPU, m_uavTextureBuffer);
	D3D11_MAPPED_SUBRESOURCE mr;

	struct ShadowTestData
	{
		unsigned int inside;
	};

	if (SUCCEEDED(DX::g_deviceContext->Map(m_uavTextureBufferCPU, 0, D3D11_MAP_READ, 0, &mr)))
	{
		ShadowTestData* data = (ShadowTestData*)mr.pData;

		m_vis = data->inside;
		DX::g_deviceContext->Unmap(m_uavTextureBufferCPU, 0);
	}
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	if (SUCCEEDED(DX::g_deviceContext->Map(m_uavTextureBufferCPU, 0, D3D11_MAP_WRITE, 0, &dataPtr)))
	{
		ShadowTestData killer = { 0 };
		memcpy(dataPtr.pData, &killer, sizeof(ShadowTestData));
		DX::g_deviceContext->CopyResource(m_uavTextureBuffer, m_uavTextureBufferCPU);
		//DX::g_deviceContext->CopyResource(m_uavTextureBuffer, m_uavTextureBufferCPU);
		DX::g_deviceContext->Unmap(m_uavTextureBufferCPU, 0);
	}
#if _DEBUG
	ImGui::Begin("PlayerVis");
	ImGui::Text("Player vis = %f", (float)m_vis);
	ImGui::End();
#endif

	//std::cout << "PlayerVis = " << m_vis << "\n"; // Remove me later
}

ID3D11UnorderedAccessView* Guard::getUAV()
{
	return m_visabilityUAV;
}

void Guard::_createUAV()
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
	hr = DX::g_device->CreateTexture2D(&TextureData, NULL, &m_uavTextureBuffer);

	TextureData.Usage = D3D11_USAGE_STAGING;
	TextureData.BindFlags = 0;
	TextureData.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	hr = DX::g_device->CreateTexture2D(&TextureData, 0, &m_uavTextureBufferCPU);

	//TextureData.Usage = D3D11_USAGE_STAGING;
	//TextureData.BindFlags = 0;
	//TextureData.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//hr = DX::g_device->CreateTexture2D(&TextureData, 0, &m_uavKILLER);

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	ZeroMemory(&UAVdesc, sizeof(UAVdesc));
	UAVdesc.Format = DXGI_FORMAT_R32_UINT;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Texture2D.MipSlice = 0;
	hr = DX::g_device->CreateUnorderedAccessView(m_uavTextureBuffer, &UAVdesc, &m_visabilityUAV);
}
