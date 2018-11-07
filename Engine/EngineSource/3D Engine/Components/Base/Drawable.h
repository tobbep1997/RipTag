#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>

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
	GuarddType = 2,
	ExcludeType = 3
};

class StaticMesh;
class DynamicMesh;
class Texture;

namespace Animation 
{
	class AnimatedModel;
};
namespace SM
{
	class AnimationStateMachine;
};

class Drawable : public Transform
{
private:
	StaticMesh* m_staticMesh = nullptr;
	DynamicMesh* m_dynamicMesh = nullptr;
	Animation::AnimatedModel* m_anim = nullptr;
	DirectX::XMFLOAT2A m_textureTileMult = DirectX::XMFLOAT2A(1.0f,1.0f);

	bool m_hidden;
public:
	SM::AnimationStateMachine* InitStateMachine();
	SM::AnimationStateMachine* m_stateMachine = nullptr;
protected:	
	Texture * p_texture;

	//Object type, is it static or dynamic
	ObjectType p_objectType;
	EntityType p_entityType;

	ID3D11Buffer * p_vertexBuffer;

	std::wstring p_vertexPath;
	std::wstring p_pixelPath;

	DirectX::XMFLOAT4A p_color;

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
	

	//Set Shaders
	virtual void setVertexShader(const std::wstring & path);
	virtual void setPixelShader(const std::wstring & path);

	virtual std::wstring getVertexPath() const;
	virtual std::wstring getPixelPath() const;

	virtual UINT getVertexSize();

	virtual ID3D11Buffer * getBuffer();


	//returns static or dynamic objtype
	virtual ObjectType getObjectType();
	virtual EntityType getEntityType();
	virtual void setEntityType(EntityType en);

	//returns AnimatedModel ptr if valid
	Animation::AnimatedModel* getAnimatedModel();

	virtual void setTextureTileMult(float u, float v);
	const DirectX::XMFLOAT2A & getTextureTileMult () const;

	virtual bool isTextureAssigned();

	virtual void setModel(StaticMesh * staticMesh);
	virtual void setModel(DynamicMesh * dynamicMesh);

	virtual void setColor(const DirectX::XMFLOAT4A & color);
	virtual void setColor(const float & x, const float & y, const float & z, const float & w);
	virtual const DirectX::XMFLOAT4A & getColor() const;

	virtual void setHidden(bool hidden);
	virtual bool getHidden();

private:
	virtual void _setStaticBuffer();
	virtual void _setDynamicBuffer();

};

