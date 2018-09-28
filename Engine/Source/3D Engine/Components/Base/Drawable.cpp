#include "Drawable.h"


void Drawable::_setStaticBuffer()
{
	DX::SafeRelease(p_vertexBuffer);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(StaticVertex) * (UINT)m_staticMesh->getVertice().size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_staticMesh->getRawVertice();
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &p_vertexBuffer);
}

void Drawable::_setDynamicBuffer()
{
	DX::SafeRelease(p_vertexBuffer);

	UINT32 vertexSize = sizeof(DynamicVertex);
	UINT32 offset = 0;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DynamicVertex) * (UINT)m_dynamicMesh->getVertices().size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_dynamicMesh->getRawVertices();
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &p_vertexBuffer);
}

void Drawable::p_createBuffer()
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

void Drawable::p_setMesh(StaticMesh * staticMesh)
{
	this->m_staticMesh = staticMesh;
}

void Drawable::p_setMesh(DynamicMesh * dynamicMesh)
{
	this->m_dynamicMesh = dynamicMesh;
}

void Drawable::setTexture(Texture * texture)
{
	this->p_texture = texture;
}



void Drawable::BindTextures()
{ 
	if (p_texture)
		p_texture->Bind(1);
}

Drawable::Drawable() : Transform()
{	
	m_staticMesh = nullptr;
	m_dynamicMesh = nullptr;
	p_vertexBuffer = nullptr;
	
}


Drawable::~Drawable()
{
	DX::SafeRelease(p_vertexBuffer);
}



void Drawable::Draw()
{
	if (this)
	{
		switch (p_objectType)
		{
		case Static:
			DX::g_geometryQueue.push_back(this);
			break;
		case Dynamic:
			DX::g_animatedGeometryQueue.push_back(this);
			break;
		}
	}
}

void Drawable::setVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Drawable::setPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}



std::wstring Drawable::getVertexPath() const
{
	return this->p_vertexPath;
}

std::wstring Drawable::getPixelPath() const
{
	return this->p_pixelPath;
}

UINT Drawable::getVertexSize()
{
	switch (p_objectType)
	{
	case Static:
		return (UINT)m_staticMesh->getVertice().size();
		break;
	case Dynamic:
		return (UINT)m_dynamicMesh->getVertices().size();
		break;
	default:
		return 0;
		break;
	}
	return 0;
	
}

ID3D11Buffer * Drawable::getBuffer()
{
	return p_vertexBuffer;
}

DirectX::XMFLOAT4X4A Drawable::getWorldmatrix()
{
	this->p_calcWorldMatrix();
	return this->p_worldMatrix;
}

ObjectType Drawable::getObjectType()
{
	return p_objectType;
}

EntityType Drawable::getEntityType()
{
	return this->p_entityType;
}

void Drawable::setEntityType(EntityType en)
{
	this->p_entityType = en;
}