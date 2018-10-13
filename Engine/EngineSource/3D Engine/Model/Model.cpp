#include "Model.h"


Model::Model() 
{

}


Model::~Model()
{

}



void Model::setModel(StaticMesh * staticMesh)
{
	/*this->p_objectType = Static;
	setVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	setPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(staticMesh);
	p_createBuffer();*/
}

void Model::setModel(DynamicMesh * dynamicMesh)
{
	/*this->p_objectType = Dynamic;
	setVertexShader(L"../Engine/Source/Shader/AnimatedVertexShader.hlsl");
	setPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	Drawable::p_setMesh(dynamicMesh);
	p_createBuffer();*/
}

