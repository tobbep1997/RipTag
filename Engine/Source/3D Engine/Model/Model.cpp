#include "Model.h"


Model::Model(ObjectType objectType, EntityType ent) :
	Drawable(objectType, ent)
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

Model::Model(ObjectType objectType, const std::string &assetFilePath, EntityType ent)
	: Drawable(objectType, ent)
{
	switch (objectType)
	{
	case Static:
		m_StaticMeshPointer = new StaticMesh();
		m_StaticMeshPointer->LoadModel(assetFilePath);
		setModel(m_StaticMeshPointer);
		setVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
		setPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
		break;
	case Dynamic:
		m_DynamicMeshPointer = new DynamicMesh();
		m_DynamicMeshPointer->LoadModel(assetFilePath);
		setModel(m_DynamicMeshPointer);
		setVertexShader(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl");
		setPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
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

void Model::_setBuffer()
{
	Drawable::CreateBuffer();
}

void Model::setModel(StaticMesh * staticMesh)
{
	Drawable::p_setMesh(staticMesh);
	this->_setBuffer();
}

void Model::setModel(DynamicMesh * dynamicMesh)
{
	Drawable::p_setMesh(dynamicMesh);
	this->_setBuffer();
}

void Model::setVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Model::setPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}
