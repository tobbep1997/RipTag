#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "../../Extern.h"
#include "../Engine/Source/3D Engine/Model/Meshes/StaticMesh.h"
#include "../Engine/Source/3D Engine/Model/Meshes/DynamicMesh.h"
#include <string>
#include "../Engine/Source/3D Engine/Model/Texture.h"
//#include "..\WICTextureLoader\WICTextureLoader.h"
#include "Transform.h"

enum ObjectType
{
	Static,
	Dynamic
};

enum EntityType
{
	DefultType = 0,
	PlayerType = 1,
	GuarddType = 2
};

class Drawable : public Transform
{
private:
	StaticMesh * m_staticMesh;
	DynamicMesh * m_dynamicMesh;

	DirectX::XMFLOAT2A m_textureTileMult = DirectX::XMFLOAT2A(1.0f,1.0f);
protected:	
	Texture * p_texture;

	//Object type, is it static or dynamic
	ObjectType p_objectType;
	EntityType p_entityType;

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
	void DrawWireFrame();
	virtual void QueueVisabilityDraw();

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
	virtual EntityType getEntityType();
	virtual void setEntityType(EntityType en);

	//returns AnimatedModel ptr if valid
	Animation::AnimatedModel* getAnimatedModel();

	virtual void setTextureTileMult(float u, float v);
	const DirectX::XMFLOAT2A & getTextureTileMult () const;

	virtual bool isTextureAssigned();

private:
	virtual void _setStaticBuffer();
	virtual void _setDynamicBuffer();

};

