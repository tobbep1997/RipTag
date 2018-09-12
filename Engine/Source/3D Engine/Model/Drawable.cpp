#include "Drawable.h"



void Drawable::_setStaticBuffer()
{
	DX::SafeRelease(m_vertexBuffer);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(StaticVertex) * (UINT)m_staticMesh->getVertice().size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_staticMesh->getRawVertice();
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer);
}

void Drawable::_setDynamicBuffer()
{
	DX::SafeRelease(m_vertexBuffer);

	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DynamicVertex) * (UINT)m_dynamicMesh->getVertices().size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_dynamicMesh->getRawVertices();
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer);
}

void Drawable::CalcWorldMatrix()
{
	using namespace DirectX;
	XMMATRIX translation	=	XMMatrixTranslation(this->p_position.x, this->p_position.y, this->p_position.z);
	XMMATRIX scaling		=	XMMatrixScaling(this->p_scale.x, this->p_scale.y, this->p_scale.z);
	XMMATRIX rotation		=	XMMatrixRotationRollPitchYaw(this->p_rotation.x, this->p_rotation.y, this->p_rotation.z);

	DirectX::XMStoreFloat4x4A(&this->p_worldMatrix, XMMatrixTranspose(rotation * scaling * translation));
}	

void Drawable::CreateBuffer()
{
	switch (p_objectType)
	{
	case Static:
		_setStaticBuffer();
		break;
	case Dynamic:
		_setDynamicBuffer();
		break;
	}
}

void Drawable::SetMesh(StaticMesh * staticMesh)
{
	this->m_staticMesh = staticMesh;
}

void Drawable::SetMesh(DynamicMesh * dynamicMesh)
{
	this->m_dynamicMesh = dynamicMesh;
}

Drawable::Drawable(ObjectType objecType) :
	m_staticMesh(nullptr),
	m_dynamicMesh(nullptr),
	m_vertexBuffer(nullptr)
{
	p_position = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_rotation = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_scale = DirectX::XMFLOAT4A(1, 1, 1, 1);


	this->p_objectType = objecType;
}


Drawable::~Drawable()
{
	DX::SafeRelease(m_vertexBuffer);
}

void Drawable::setPosition(DirectX::XMFLOAT4A pos)
{
	this->p_position = pos;
}

void Drawable::setPosition(float x, float y, float z, float w)
{
	this->setPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void Drawable::setRotation(DirectX::XMFLOAT4A rot)
{
	this->p_rotation = rot;
}

void Drawable::setRotation(float x, float y, float z)
{
	this->setRotation(DirectX::XMFLOAT4A(x, y, z, 1));
}

void Drawable::addRotation(float x, float y, float z)
{
	p_rotation.x += x;
	p_rotation.y += y;
	p_rotation.z += z;
}

void Drawable::setScale(DirectX::XMFLOAT4A scale)
{
	this->p_scale = scale;
}

void Drawable::setScale(float x, float y, float z, float w)
{
	this->setScale(DirectX::XMFLOAT4A(x, y, z, w));
}



void Drawable::Draw()
{
	DX::g_geometryQueue.push_back(this);
}

std::wstring Drawable::getVertexPath() const
{
	return this->p_vertexPath;
}

std::wstring Drawable::getPixelPath() const
{
	return this->p_pixelPath;
}

UINT Drawable::VertexSize()
{
	switch (p_objectType)
	{
	case Static:
		return (UINT)m_staticMesh->getVertice().size();
		break;
	case Dynamic:
		return (UINT)m_dynamicMesh->getVertices().size();
		break;
	}
	
}

ID3D11Buffer * Drawable::getBuffer()
{
	return m_vertexBuffer;
}

DirectX::XMFLOAT4X4A Drawable::getWorldmatrix()
{
	this->CalcWorldMatrix();
	return this->p_worldMatrix;
}
