#include "CubePrototype.h"

CubePrototype::CubePrototype()
{
	this->s = new StaticMesh();
	this->s->LoadModel("../Assets/sphere.bin");

	this->m = Model(ObjectType::Static);
	this->m.SetVertexShader(L"../Engine/Source/Shader/VertexShader.hlsl");
	this->m.SetPixelShader(L"../Engine/Source/Shader/PixelShader.hlsl");
	this->m.setPosition(0, 0, 0);
	this->m.SetModel(s);
	this->m.setPosition(0, -3, 0);
	this->m.setScale(1, 1, 1);
}

CubePrototype::~CubePrototype()
{
	delete this->s;
}

void CubePrototype::setPosition(DirectX::XMFLOAT4A pos)
{
	this->m.setPosition(DirectX::XMFLOAT4A(pos.x, pos.y, pos.z + 2, pos.w));
}

void CubePrototype::Draw()
{
	this->m.Draw();
}
