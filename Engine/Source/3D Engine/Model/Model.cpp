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

	switch (objectType)
	{
	case Static:
		m_StaticMeshPointer = new StaticMesh();
		m_StaticMeshPointer->LoadModel(tempString);
		SetModel(m_StaticMeshPointer);
		SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
		SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
		assetName = assetFilePath;
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

void Model::setTexture(const std::string & filepath)
{
	std::wstring texString;
	texString.assign(filepath.begin(), filepath.end());
	m_texture.Load(texString.c_str());
	m_texture.Bind(1);
	//Drawable::setTextures(texString);
}



void Model::SetVertexShader(const std::wstring & path)
{
	this->p_vertexPath = path;
}

void Model::SetPixelShader(const std::wstring & path)
{
	this->p_pixelPath = path;
}
