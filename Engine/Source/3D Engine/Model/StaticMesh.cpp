#include "StaticMesh.h"



StaticMesh::StaticMesh()
{
}


StaticMesh::~StaticMesh()
{
}

const StaticVertex * StaticMesh::getRawVertice() const
{
	return staticVertex.data();
}

const std::vector<StaticVertex>& StaticMesh::getVertice() const
{
	return staticVertex;
}

void StaticMesh::setVertices(std::vector<StaticVertex>& input)
{
	staticVertex.clear();
	staticVertex = input;
}

void StaticMesh::SET_DEFAULT()
{
	using namespace DirectX;

	MyLibrary::Loadera meshloader;
	MyLibrary::MeshFromFile newMesh = meshloader.readMeshFile("kub.bin");

	StaticVertex tempvertex;
	for (int i = 0; i < newMesh.mesh_nrOfVertices; i++)
	{
		
		tempvertex.pos.x = newMesh.mesh_vertices->vertex_position[0];
		tempvertex.pos.y = newMesh.mesh_vertices->vertex_position[1];
		tempvertex.pos.z = newMesh.mesh_vertices->vertex_position[2];
		tempvertex.pos.w = 1.0f;
		
		tempvertex.normal.x = 0.0f;
		tempvertex.normal.y = 0.0f;
		tempvertex.normal.z = 0.0f;
		tempvertex.normal.w = 0.0f;
		
		tempvertex.uvPos.x = newMesh.mesh_vertices->vertex_UVCoord[0];
		tempvertex.uvPos.y = newMesh.mesh_vertices->vertex_UVCoord[1];
		
		tempvertex.tangent.x = newMesh.mesh_vertices->vertex_tangent[0];
		tempvertex.tangent.y = newMesh.mesh_vertices->vertex_tangent[0];
		tempvertex.tangent.z  = newMesh.mesh_vertices->vertex_tangent[0];
		tempvertex.tangent.x = 0.0f;

		staticVertex.push_back(tempvertex);

	}
}
