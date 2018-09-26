#pragma once
#include "../Engine/Source/3D Engine/Model/Model.h"

class CubePrototype
{
public:
	CubePrototype();
	~CubePrototype();

	void setPosition(DirectX::XMFLOAT4A pos);
	void setModel();

private:
	Model m;
	StaticMesh * s;
};