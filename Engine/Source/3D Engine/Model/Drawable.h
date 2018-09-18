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
#include <string>

class Texture;

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

	//Textures
	Texture* m_diffuseTexture = nullptr;
	Texture* m_normalTexture = nullptr;
	Texture* m_MRATexture = nullptr;

	void _setStaticBuffer();
	void _setDynamicBuffer();


protected:
	//Information for worldMatrix
	DirectX::XMFLOAT4A p_position;
	DirectX::XMFLOAT4A p_scale;
	DirectX::XMFLOAT4A p_rotation;

	//WorldMatrix. scale * rot* translateopn
	DirectX::XMFLOAT4X4A p_worldMatrix;

	//Calculates the worldMatrix
	void CalcWorldMatrix();

	//Object type, is it static or dynamic
	ObjectType p_objectType;

	ID3D11Buffer * m_vertexBuffer;

	std::wstring p_vertexPath;
	std::wstring p_pixelPath;

	void CreateBuffer();

	//Setting the mesh for the object
	void SetMesh(StaticMesh * staticMesh);
	void SetMesh(DynamicMesh * dynamicMesh);

	//Texture stuff

public:
	Drawable(ObjectType ObjecType = ObjectType::Static);
	virtual ~Drawable();

	void setPosition(DirectX::XMFLOAT4A pos);
	void setPosition(float x, float y, float z, float w = 1);

	void setRotation(DirectX::XMFLOAT4A rot);
	void setRotation(float x, float y, float z);
	void addRotation(float x = 0, float y = 0, float z = 0);
											  
	void setScale(DirectX::XMFLOAT4A scale);
	void setScale(float x = 1, float y = 1, float z = 1, float w = 1);

	const DirectX::XMFLOAT4A & getPosition() const;
	void BindTextures();
	void SetTextures(Texture* diffuseTexture = nullptr, Texture* normalTexture = nullptr, Texture* MRATexture = nullptr);
	
	void Draw();
	void DrawAnimated();
	std::wstring getVertexPath() const;
	std::wstring getPixelPath() const;

	UINT VertexSize();

	ID3D11Buffer * getBuffer();

	//Returns the objects worldMatrix
	DirectX::XMFLOAT4X4A getWorldmatrix();

	//returns static or dynamic objtype
	ObjectType getObjectType();



};

