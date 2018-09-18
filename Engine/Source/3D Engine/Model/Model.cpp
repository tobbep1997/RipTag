#include "Model.h"


Model::Model(ObjectType objectType) : 
	Drawable(objectType)
{
	switch (objectType)
	{
	case Static:
		break;
	case Dynamic:
		break;
	default:
		break;
	}
}

Model::Model(ObjectType objectType, const std::string &assetFilePath) 
	: Drawable(objectType)
{
	switch (objectType)
	{
	case Static:
		m_StaticMeshPointer = new StaticMesh();
		m_StaticMeshPointer->LoadModel(assetFilePath);
		SetModel(m_StaticMeshPointer);
		SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
		SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
		break;
	case Dynamic:
		m_DynamicMeshPointer = new DynamicMesh();
		m_DynamicMeshPointer->LoadModel(assetFilePath);
		SetModel(m_DynamicMeshPointer);
		SetVertexShader(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl");
		SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
		break;
	default:
		break;
	}

}

Model::~Model()
{
	delete m_StaticMeshPointer;
	delete m_DynamicMeshPointer;
}

void Model::SetBuffer()
{
	Drawable::CreateBuffer();
}

void Model::SetModel(StaticMesh * staticMesh)
{
	Drawable::SetMesh(staticMesh);
	this->SetBuffer();
}

void Model::SetModel(DynamicMesh * dynamicMesh)
{
	Drawable::SetMesh(dynamicMesh);
	this->SetBuffer();
}

void Model::SetVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Model::SetPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}
