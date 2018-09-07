#include "Model.h"



Model::Model(ObjectType objectType) : 
	Drawable(objectType)
{
}


Model::~Model()
{
}

void Model::SetBuffer()
{
	Drawable::SetBuffer();
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
