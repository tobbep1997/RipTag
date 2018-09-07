#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "../Extern.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
enum ObjectType
{
	Static,
	Dynamic
};

class Drawable
{
private:
	StaticMesh * m_staticMesh;
	DynamicMesh * m_dynamicMesh;

	void _setStaticBuffer();
	void _setDynamicBuffer();

protected:
	DirectX::XMFLOAT4A p_position;
	DirectX::XMFLOAT4A p_scale;
	DirectX::XMFLOAT4A p_rotation;

	ObjectType p_objectType;

	DirectX::XMFLOAT4X4A p_worldMatrix;

	ID3D11Buffer * m_vertexBuffer;

	void CalcMatrix();

	void SetBuffer();

	void SetMesh(StaticMesh * staticMesh);
	void SetMesh(DynamicMesh * dynamicMesh);

public:
	Drawable(ObjectType ObjecType = ObjectType::Static);
	virtual ~Drawable();

	void Draw();

	UINT VertexSize();

	ID3D11Buffer * getBuffer();

	//Returns the objects worldMatrix
	DirectX::XMFLOAT4X4A getWorldmatrix();
};

