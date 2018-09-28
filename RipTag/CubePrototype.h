#pragma once
#include "NetworkIDObject.h"
#include "../Engine/Source/3D Engine/Model/Model.h"

class CubePrototype : public RakNet::NetworkIDObject
{
public:
	CubePrototype();
	~CubePrototype();

	void setPosition(DirectX::XMFLOAT4A pos);
	void Draw();
	
private:
	Model m;
	StaticMesh * s;
};