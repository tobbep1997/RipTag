#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "../Extern.h"
#include "../Model/StaticMesh.h"
#include "../Model/DynamicMesh.h"
#include <string>
#include "../Model/Texture.h"
#include "..\WICTextureLoader\WICTextureLoader.h"
#include "Transform.h"

enum ObjectType
{
	Static,
	Dynamic
};

class Drawable : public Transform
{
private:
	StaticMesh * m_staticMesh;
	DynamicMesh * m_dynamicMesh;
protected:	
	Texture * p_texture;

	//Object type, is it static or dynamic
	ObjectType p_objectType;

	ID3D11Buffer * p_vertexBuffer;

	std::wstring p_vertexPath;
	std::wstring p_pixelPath;

	virtual void p_createBuffer();

	//Setting the mesh for the object
	virtual void p_setMesh(StaticMesh * staticMesh);
	virtual void p_setMesh(DynamicMesh * dynamicMesh);

	//Texture stuff

public:
	

	Drawable();
	virtual ~Drawable();
	virtual void setTexture(Texture * texture);	

	virtual void BindTextures();
	
	virtual void Draw();

	//Set Shaders
	virtual void setVertexShader(const std::wstring & path);
	virtual void setPixelShader(const std::wstring & path);

	virtual std::wstring getVertexPath() const;
	virtual std::wstring getPixelPath() const;

	virtual UINT getVertexSize();

	virtual ID3D11Buffer * getBuffer();

	//Returns the objects worldMatrix
	virtual DirectX::XMFLOAT4X4A getWorldmatrix();

	//returns static or dynamic objtype
	virtual ObjectType getObjectType();

private:
	virtual void _setStaticBuffer();
	virtual void _setDynamicBuffer();

};

