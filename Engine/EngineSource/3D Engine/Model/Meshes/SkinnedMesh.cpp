#include "EnginePCH.h"
#include "SkinnedMesh.h"




SkinnedMesh::SkinnedMesh()
{
}


SkinnedMesh::~SkinnedMesh()
{
}

const DynamicVertex * SkinnedMesh::getRawVertices() const
{
	return m_dynamicVertex.data();
}

const std::vector<DynamicVertex>& SkinnedMesh::getVertices() const
{
	return m_dynamicVertex;
}

void SkinnedMesh::setVertices(std::vector<DynamicVertex>& input)
{
	m_dynamicVertex.clear();
	m_dynamicVertex = input;
}

void SkinnedMesh::SET_DEFAULT()
{
	using namespace DirectX;

	ImporterLibrary::CustomFileLoader meshloader;
	ImporterLibrary::SkinnedMeshFromFile newMesh = meshloader.readSkinnedMeshFile("../Assets/ANIMATEDCUBE.bin");

	DynamicVertex tempvertex;
	for (unsigned int i = 0; i < newMesh.mesh_nrOfVertices; i++)
	{
		//TODO: G�r ordentligt.
		//Please do
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

		tempvertex.influencingJoint.x = newMesh.mesh_vertices[i].influencing_joint[0];
		tempvertex.influencingJoint.y = newMesh.mesh_vertices[i].influencing_joint[1];
		tempvertex.influencingJoint.z = newMesh.mesh_vertices[i].influencing_joint[2];
		tempvertex.influencingJoint.w = newMesh.mesh_vertices[i].influencing_joint[3];
		
		tempvertex.jointWeights.x = newMesh.mesh_vertices[i].joint_weights[0];
		tempvertex.jointWeights.y = newMesh.mesh_vertices[i].joint_weights[1];
		tempvertex.jointWeights.z = newMesh.mesh_vertices[i].joint_weights[2];
		tempvertex.jointWeights.w = newMesh.mesh_vertices[i].joint_weights[3];

		m_dynamicVertex.push_back(tempvertex);

	}
	delete newMesh.mesh_vertices;
}

void SkinnedMesh::setName(const std::string & name)
{
	this->m_meshName = name;
}

const std::string & SkinnedMesh::getName() const
{
	return this->m_meshName;
}

void SkinnedMesh::LoadMesh(const std::string & path)
{
	using namespace DirectX;

	ImporterLibrary::CustomFileLoader meshloader;
	ImporterLibrary::SkinnedMeshFromFile newMesh = meshloader.readSkinnedMeshFile(path);
	DynamicVertex tempvertex;
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

		tempvertex.influencingJoint.x = newMesh.mesh_vertices[i].influencing_joint[0];
		tempvertex.influencingJoint.y = newMesh.mesh_vertices[i].influencing_joint[1];
		tempvertex.influencingJoint.z = newMesh.mesh_vertices[i].influencing_joint[2];
		tempvertex.influencingJoint.w = newMesh.mesh_vertices[i].influencing_joint[3];

		tempvertex.jointWeights.x = newMesh.mesh_vertices[i].joint_weights[0];
		tempvertex.jointWeights.y = newMesh.mesh_vertices[i].joint_weights[1];
		tempvertex.jointWeights.z = newMesh.mesh_vertices[i].joint_weights[2];
		tempvertex.jointWeights.w = newMesh.mesh_vertices[i].joint_weights[3];

		m_dynamicVertex.push_back(tempvertex);

	}
	delete newMesh.mesh_vertices;
}
