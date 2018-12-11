#include "EnginePCH.h"
#include "StaticMesh.h"


void StaticMesh::_createVertexBuffer()
{
	DX::SafeRelease(m_vertexBuffer);

	UINT32 vertexSize = sizeof(StaticVertex);
	UINT32 offset = 0;
	m_staticVertex.shrink_to_fit();
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(StaticVertex) * (UINT)getVertice().size();


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = getRawVertice();
	HRESULT hr;
	if (SUCCEEDED(hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &m_vertexBuffer)))
	{
		DX::SetName(m_vertexBuffer, "StaticMesh: m_vertexBuffer");
	}
}

StaticMesh::StaticMesh()
{
}


StaticMesh::~StaticMesh()
{
	if (m_collisionBox)
	{
		delete[] m_collisionBox->boxes;
		delete m_collisionBox;
	}
	if (m_vertexBuffer)
		m_vertexBuffer->Release();
}

const StaticVertex * StaticMesh::getRawVertice() const
{
	return m_staticVertex.data();
}

const std::vector<StaticVertex>& StaticMesh::getVertice() const
{
	return m_staticVertex;
}

void StaticMesh::setVertices(std::vector<StaticVertex>& input)
{
	m_staticVertex.clear();
	m_staticVertex = input;
	_createVertexBuffer();
	m_staticVertex.shrink_to_fit();
}

const ImporterLibrary::CollisionBoxes & StaticMesh::getCollisionBoxes() const
{
	return *m_collisionBox;
}

void StaticMesh::SET_DEFAULT()
{
	using namespace DirectX;

	ImporterLibrary::CustomFileLoader meshloader;
	ImporterLibrary::MeshFromFile newMesh = meshloader.readMeshFile("../Assets/sphere.bin");

	StaticVertex tempvertex;
	m_staticVertex.reserve(newMesh.mesh_nrOfVertices);
	for (unsigned int i = 0; i < newMesh.mesh_nrOfVertices; i++)
	{
		
		tempvertex.pos.x = newMesh.mesh_vertices[i].vertex_position[0];
		tempvertex.pos.y = newMesh.mesh_vertices[i].vertex_position[1];
		tempvertex.pos.z = newMesh.mesh_vertices[i].vertex_position[2];
		tempvertex.pos.w = 1.0f;
		
		tempvertex.normal.x = newMesh.mesh_vertices[i].vertex_normal[0];
		tempvertex.normal.y = newMesh.mesh_vertices[i].vertex_normal[1];
		tempvertex.normal.z = newMesh.mesh_vertices[i].vertex_normal[2];
		tempvertex.normal.w = 0.0f;
		
		tempvertex.uvPos.x = newMesh.mesh_vertices[i].vertex_UVCoord[0];
		tempvertex.uvPos.y = newMesh.mesh_vertices[i].vertex_UVCoord[1];
		
		tempvertex.tangent.x = newMesh.mesh_vertices[i].vertex_tangent[0];
		tempvertex.tangent.y = newMesh.mesh_vertices[i].vertex_tangent[1];
		tempvertex.tangent.z = newMesh.mesh_vertices[i].vertex_tangent[2];
		tempvertex.tangent.x = 0.0f;

		m_staticVertex.push_back(tempvertex);

	}
	m_staticVertex.shrink_to_fit();
	delete newMesh.mesh_vertices;
	_createVertexBuffer();
}

void StaticMesh::setName(const std::string & name)
{
	this->m_meshName = name;
}

const std::string & StaticMesh::getName() const
{
	return this->m_meshName;
}

bool Close(const DirectX::XMFLOAT4A & p1, const DirectX::XMFLOAT4A & p2, const float & distance) 
{
	const float d = powf(fabs(p1.x - p2.x), 2) + powf(fabs(p1.y - p2.y), 2) + powf(fabs(p1.z - p2.z), 2);

	return d <= powf(distance, 2.0f);
}

DirectX::XMFLOAT4A CalcNewNormal(const std::vector<DirectX::XMFLOAT4A> & normals)
{
	DirectX::XMFLOAT4A newNormal = DirectX::XMFLOAT4A(0,0,0,0);
	for (int i = 0; i < normals.size(); i++)
	{
		newNormal.x += normals[i].x;
		newNormal.y += normals[i].y;
		newNormal.z += normals[i].z;

	}
	newNormal.x = newNormal.x / static_cast<float>(normals.size());
	newNormal.y = newNormal.y / static_cast<float>(normals.size());
	newNormal.z = newNormal.z / static_cast<float>(normals.size());

	return newNormal;
}

void StaticMesh::LoadMesh(const std::string & path)
{
	using namespace DirectX;

	ImporterLibrary::CustomFileLoader meshloader;
	ImporterLibrary::MeshFromFile newMesh = meshloader.readMeshFile(path);

	StaticVertex tempvertex;
	//m_meshName = newMesh.mesh_meshID;
	m_staticVertex.reserve(newMesh.mesh_nrOfVertices);
	for (unsigned int i = 0; i < newMesh.mesh_nrOfVertices; i++)
	{

		tempvertex.pos.x = newMesh.mesh_vertices[i].vertex_position[0];
		tempvertex.pos.y = newMesh.mesh_vertices[i].vertex_position[1];
		tempvertex.pos.z = newMesh.mesh_vertices[i].vertex_position[2];
		tempvertex.pos.w = 1.0f;

		tempvertex.normal.x = newMesh.mesh_vertices[i].vertex_normal[0];
		tempvertex.normal.y = newMesh.mesh_vertices[i].vertex_normal[1];
		tempvertex.normal.z = newMesh.mesh_vertices[i].vertex_normal[2];
		tempvertex.normal.w = 0.0f;

		tempvertex.tangent.x = newMesh.mesh_vertices[i].vertex_tangent[0];
		tempvertex.tangent.y = newMesh.mesh_vertices[i].vertex_tangent[1];
		tempvertex.tangent.z = newMesh.mesh_vertices[i].vertex_tangent[2];
		tempvertex.tangent.w = 0.0f;

		tempvertex.uvPos.x = newMesh.mesh_vertices[i].vertex_UVCoord[0];
		tempvertex.uvPos.y = newMesh.mesh_vertices[i].vertex_UVCoord[1];
		tempvertex.uvPos.z = 0; 
		tempvertex.uvPos.w = 0;

		m_staticVertex.push_back(tempvertex);

	}
	using namespace DirectX;
	for (size_t i = 0; i < m_staticVertex.size(); i++)
	{
		std::vector<DirectX::XMFLOAT4A> normals = std::vector<DirectX::XMFLOAT4A>(m_staticVertex.size() / 3U);
		for (size_t j = i + 1; j < m_staticVertex.size(); j++)
		{
			if (Close(m_staticVertex[i].pos, m_staticVertex[j].pos, 0.01f))
			{
				normals.push_back(m_staticVertex[i].normal);
			}	
		}
		if (!normals.empty())
		{
			m_staticVertex[i].normal = CalcNewNormal(normals);
			normals.clear();
		}
	}

	m_staticVertex.shrink_to_fit();
	delete newMesh.mesh_vertices;
	_createVertexBuffer();
}

void StaticMesh::LoadCollision(const std::string & path)
{
	ImporterLibrary::CustomFileLoader meshloader;
	m_collisionBox = new ImporterLibrary::CollisionBoxes();
	*m_collisionBox = meshloader.readMeshCollisionBoxes(path);
}

ID3D11Buffer* StaticMesh::getBuffer()
{
	return this->m_vertexBuffer;
}
