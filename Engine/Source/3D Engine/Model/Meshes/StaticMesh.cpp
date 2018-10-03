#include "StaticMesh.h"



StaticMesh::StaticMesh()
{
}


StaticMesh::~StaticMesh()
{

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
}

void StaticMesh::SET_DEFAULT()
{
	using namespace DirectX;

	MyLibrary::Loadera meshloader;
	MyLibrary::MeshFromFile newMesh = meshloader.readMeshFile("../Assets/sphere.bin");

	StaticVertex tempvertex;
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
	delete newMesh.mesh_vertices;
}

void StaticMesh::setName(const std::string & name)
{
	this->m_meshName = name;
}

const std::string & StaticMesh::getName() const
{
	return this->m_meshName;
}

void StaticMesh::LoadMesh(const std::string & path)
{
	using namespace DirectX;

	MyLibrary::Loadera meshloader;
	MyLibrary::MeshFromFile newMesh = meshloader.readMeshFile(path);

	StaticVertex tempvertex;
	//m_meshName = newMesh.mesh_meshID;
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
		tempvertex.tangent.w = 0.0f;

		m_staticVertex.push_back(tempvertex);

	}
	delete newMesh.mesh_vertices;
}
