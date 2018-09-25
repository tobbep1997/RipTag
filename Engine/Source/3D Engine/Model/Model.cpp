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
	std::string tempString = "../Assets/";
	tempString.append(assetFilePath + "FOLDER/" + assetFilePath + ".bin");

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
	this->p_objectType = Static;
	SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	Drawable::SetMesh(staticMesh);
	this->SetBuffer();
}

void Model::SetModel(DynamicMesh * dynamicMesh)
{
	this->p_objectType = Dynamic;
	SetVertexShader(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl");
	SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	Drawable::SetMesh(dynamicMesh);
	this->SetBuffer();
}

void Model::setTexture(Texture* textureToset)
{
	this->p_texture = textureToset;
}

void Model::SetVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Model::SetPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}

void Model::bindTexture(int slot)
{
	this->p_texture->Bind(slot);
}
