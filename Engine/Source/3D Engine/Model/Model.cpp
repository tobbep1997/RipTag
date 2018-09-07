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


Model::~Model()
{
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
