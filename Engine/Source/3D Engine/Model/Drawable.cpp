#include "Drawable.h"
#include "Texture.h"


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

void Drawable::p_calcWorldMatrix()
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

void Drawable::p_setMesh(StaticMesh * staticMesh)
{
	this->m_staticMesh = staticMesh;
}

void Drawable::p_setMesh(DynamicMesh * dynamicMesh)
{
	this->m_dynamicMesh = dynamicMesh;
}

void Drawable::setTextures(const std::wstring & filePath)
{
	std::wstring albedoName = L"../Assets/";
	albedoName.append(filePath + L"FOLDER/" + filePath + L"_ALBEDO.png");
	std::wstring ORMname = L"../Assets/";
	ORMname.append(filePath + L"FOLDER/" + filePath + L"_ORM.png");
	std::wstring normalName = L"../Assets/";
	normalName.append(filePath + L"FOLDER/" + filePath + L"_NORMAL.png");
	
	//CD3D11_TEXTURE2D_DESC texDesc();
	//m_diffuseTexture[0]->Load(albedoName.c_str());
	//m_diffuseTexture[0]->Bind(1);
	//HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, albedoName.c_str(), nullptr, &m_diffuseTexture[0].);
	//hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, normalName.c_str(), nullptr, &m_meshTextures.MeshTextureSRV[1]);
	//hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, ORMname.c_str(), nullptr, &m_meshTextures.MeshTextureSRV[2]);
	//DX::SafeRelease(tempMeshTextureSRV);
	//hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, albedoName.c_str(), nullptr, &tempMeshTextureSRV);
	//DX::g_deviceContext->PSSetShaderResources(1, 1, &tempMeshTextureSRV);
//	DX::g_deviceContext->PSSetShaderResources(2, 3, &m_meshTextures.MeshTextureSRV[1]);
//	DX::g_deviceContext->PSSetShaderResources(3, 3, &m_meshTextures.MeshTextureSRV[2]);
	
}

//void Drawable::SetTextures(Texture* diffuseTexture /*= nullptr*/, Texture* normalTexture /*= nullptr*/, Texture* MRATexture /*= nullptr*/)
//{
//	m_diffuseTexture = diffuseTexture;
//	m_MRATexture = MRATexture;
//	m_normalTexture = normalTexture;
//}

void Drawable::BindTextures()
{ //TODO Optimize (one call for all)
	p_texture->Bind(1);
}

Drawable::Drawable(ObjectType objectType, EntityType ent) :
	m_staticMesh(nullptr),
	m_dynamicMesh(nullptr),
	p_vertexBuffer(nullptr)
{
	p_position = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_rotation = DirectX::XMFLOAT4A(0, 0, 0, 1);
	p_scale = DirectX::XMFLOAT4A(1, 1, 1, 1);


	this->p_objectType = objectType;
	this->p_entityType = ent;
}


Drawable::~Drawable()
{
	DX::SafeRelease(p_vertexBuffer);
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

const DirectX::XMFLOAT4A & Drawable::getPosition() const
{
	return p_position;
}



void Drawable::Draw()
{
	DX::g_geometryQueue.push_back(this);
}

void Drawable::DrawAnimated()
{
	DX::g_animatedGeometryQueue.push_back(this);
}

void Drawable::QueueVisabilityDraw()
{
	DX::g_visabilityDrawQueue.push_back(this);
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
	return p_entityType;
}

void Drawable::SetEntityType(EntityType en)
{
	this->p_entityType = en;
}
