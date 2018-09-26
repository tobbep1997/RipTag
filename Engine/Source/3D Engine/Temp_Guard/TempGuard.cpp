#include "TempGuard.h"
Guard::Guard()
{
	m_frustum.setPoints();
	m_pos = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
	m_rot = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
	//m_cam.setDirection(0.0f, 0.0f, -1.0f);
	m_cam = Camera(DirectX::XM_PI * 0.5f, 1.0f);
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
}

Guard::~Guard()
{
	DX::SafeRelease(m_vertexBuffer);
	
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
