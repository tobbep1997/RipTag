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
	StaticVertex v[3];

	v[0].pos = XMFLOAT4A(0.0f, 0.5f, 0.0f, 1.0f);
	v[0].normal = XMFLOAT4A(0.0f, 0.0f, 1.0f, 1.0f);
	v[0].tangent = XMFLOAT4A(0, 0, 0, 0);
	v[0].uvPos = XMFLOAT2A(0, 0);

	v[1].pos = XMFLOAT4A(0.5f, -0.5f, 0.0f, 1.0f);
	v[1].normal = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	v[1].tangent = XMFLOAT4A(0, 0, 0, 0);
	v[1].uvPos = XMFLOAT2A(0, 0);

	v[2].pos = XMFLOAT4A(-0.5f, -0.5f, 0.0f, 1.0f);
	v[2].normal = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
	v[2].tangent = XMFLOAT4A(0, 0, 0, 0);
	v[2].uvPos = XMFLOAT2A(0, 0);

	staticVertex.push_back(v[0]);
	staticVertex.push_back(v[1]);
	staticVertex.push_back(v[2]);
}
