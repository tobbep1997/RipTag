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
#include "Texture.h"
#include "..\WICTextureLoader\WICTextureLoader.h"
//struct Texture
//{
//	ID3D11ShaderResourceView* MeshTextureSRV;
//};
enum ObjectType
{
	Static,
	Dynamic
};

enum EntityType
{
	Defult = 0,
	Player = 1,
	Guardd = 2
};

class Drawable
{
private:
	StaticMesh * m_staticMesh;
	DynamicMesh * m_dynamicMesh;
//	Texture m_meshTextures;

	//Textures
	
	//Texture* m_normalTexture = nullptr;
	//Texture* m_MRATexture = nullptr;



protected:
	//Information for worldMatrix
	DirectX::XMFLOAT4A p_position;
	DirectX::XMFLOAT4A p_scale;
	DirectX::XMFLOAT4A p_rotation;
	Texture * p_texture;
	//WorldMatrix. scale * rot* translateopn
	DirectX::XMFLOAT4X4A p_worldMatrix;

	//Calculates the worldMatrix
	void p_calcWorldMatrix();

	//Object type, is it static or dynamic
	ObjectType p_objectType;
	EntityType p_entityType;

	ID3D11Buffer * p_vertexBuffer;

	std::wstring p_vertexPath;
	std::wstring p_pixelPath;

	void CreateBuffer();

	//Setting the mesh for the object
	void p_setMesh(StaticMesh * staticMesh);
	void p_setMesh(DynamicMesh * dynamicMesh);

	//Texture stuff

public:
	Drawable(ObjectType ObjecType = ObjectType::Static, EntityType ent = EntityType::Defult);
	virtual ~Drawable();
	void setTextures(const std::wstring& filePath);
	
	void setPosition(DirectX::XMFLOAT4A pos);
	void setPosition(float x, float y, float z, float w = 1);

	void setRotation(DirectX::XMFLOAT4A rot);
	void setRotation(float x, float y, float z);
	void addRotation(float x = 0, float y = 0, float z = 0);
											  
	void setScale(DirectX::XMFLOAT4A scale);
	void setScale(float x = 1, float y = 1, float z = 1, float w = 1);

	const DirectX::XMFLOAT4A & getPosition() const;
	void BindTextures();
	//void SetTextures(Texture* diffuseTexture = nullptr, Texture* normalTexture = nullptr, Texture* MRATexture = nullptr);
	
	void Draw();
	void DrawAnimated();

	//MIGHT BE CHANGED
	void QueueVisabilityDraw();

	std::wstring getVertexPath() const;
	std::wstring getPixelPath() const;

	UINT getVertexSize();

	ID3D11Buffer * getBuffer();

	//Returns the objects worldMatrix
	DirectX::XMFLOAT4X4A getWorldmatrix();

	//returns static or dynamic objtype
	ObjectType getObjectType();
	EntityType getEntityType();
	void SetEntityType(EntityType en);

private:
	void _setStaticBuffer();
	void _setDynamicBuffer();

};

