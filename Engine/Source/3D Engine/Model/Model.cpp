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

void Model::_setBuffer()
{
	Drawable::CreateBuffer();
}

void Model::setModel(StaticMesh * staticMesh)
{
	this->p_objectType = Static;
	setVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	setPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(staticMesh);
	this->_setBuffer();
}

void Model::setModel(DynamicMesh * dynamicMesh)
{
	this->p_objectType = Dynamic;
	setVertexShader(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl");
	setPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(dynamicMesh);
	this->_setBuffer();
}

void Model::setTexture(Texture* textureToset)
{
	this->p_texture = textureToset;
}

void Model::setVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Model::setPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}

void Model::bindTexture(int slot)
{
	this->p_texture->Bind(slot);
}
