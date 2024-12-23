#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>
#include <DirectXCollision.h>
#include "Transform.h"
#include <d3d11_3.h>

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
	ExcludeType = 3,
	RemotePlayerType = 4,
	FirstPersonPlayer = 5,
	CurrentGuard = 6
};

class StaticMesh;
class SkinnedMesh;
class Texture;

namespace Animation 
{
	class AnimationPlayer;
};
namespace SM
{
	class AnimationStateMachine;
};

class Drawable : public Transform
{
private:
	StaticMesh* m_staticMesh = nullptr;
	SkinnedMesh* m_skinnedMesh = nullptr;
	Animation::AnimationPlayer* m_anim = nullptr;
	DirectX::XMFLOAT2A m_textureTileMult = DirectX::XMFLOAT2A(1.0f,1.0f);

	float m_destructionRate;
	DirectX::XMINT4 m_typeOfAbilityEffect = { 0, 0, 0, 0 };

	bool m_hidden;
	bool m_outline;
	bool m_transparant;
	bool m_castShadow;
	bool m_destroyState = false;
	DirectX::XMFLOAT4X4A m_lastTransformation;

	DirectX::XMFLOAT4A m_outLineColor;

	ID3D11Buffer* uavstage = nullptr;
	ID3D11Buffer * m_UAVOutput = nullptr;
	ID3D11UnorderedAccessView* m_animatedUAV = nullptr;

protected:	
	Texture * p_texture;

	//Object type, is it static or dynamic
	ObjectType p_objectType;
	EntityType p_entityType;

	DirectX::BoundingBox * m_bb = nullptr;
	DirectX::XMFLOAT3 m_extens;
	ID3D11Buffer * p_vertexBuffer = nullptr;

	std::wstring p_vertexPath;
	std::wstring p_pixelPath;

	DirectX::XMFLOAT4A p_color;

	virtual void p_createBuffer();

	//Setting the mesh for the object
	virtual void p_setMesh(StaticMesh * staticMesh);
	virtual void p_setMesh(SkinnedMesh * skinnedMesh);
	

public:
	virtual void p_createBoundingBox(const DirectX::XMFLOAT3 & center, const DirectX::XMFLOAT3 & extens);
	virtual void p_createBoundingBox(const DirectX::XMFLOAT3 & extens);
	virtual void setBoundingBoxSizeForDynamicObjects(const DirectX::XMFLOAT3 & extens);
	

	Drawable();
	virtual ~Drawable();
	virtual void setTexture(Texture * texture);	
	virtual Texture * getTexture() const;

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

	//returns AnimationPlayer ptr if valid
	Animation::AnimationPlayer* getAnimationPlayer();
	StaticMesh* getStaticMesh();

	virtual void setTextureTileMult(float u, float v);
	const DirectX::XMFLOAT2A & getTextureTileMult () const;

	virtual bool isTextureAssigned();

	virtual void setModel(StaticMesh * staticMesh);
	virtual void setModel(SkinnedMesh * skinnedMesh);

	virtual void setColor(const DirectX::XMFLOAT4A & color);
	virtual void setColor(const float & x, const float & y, const float & z, const float & w);
	virtual const DirectX::XMFLOAT4A & getColor() const;

	virtual void setHidden(bool hidden);
	virtual bool getHidden();

	virtual void setOutline(bool outline);
	virtual bool getOutline();

	virtual void setOutlineColor(const DirectX::XMFLOAT4A & color);
	virtual DirectX::XMFLOAT4A getOutlineColor();
	virtual DirectX::BoundingBox * getBoundingBox();

	virtual void SetTransparant(const bool & bo);
	virtual bool GetTransparant();

	virtual void setDestroyState(const bool newState) { this->m_destroyState = newState; };
	virtual const bool getDestroyState() { return this->m_destroyState; };

	std::string getTextureName() const;

	virtual void setDestructionRate(const float updateRate) { this->m_destructionRate = updateRate; };
	virtual const float getDestructionRate() { return this->m_destructionRate; };

	virtual void setLastTransform(const DirectX::XMFLOAT4X4A lastPos) { this->m_lastTransformation = lastPos; };
	virtual const DirectX::XMFLOAT4X4A getLastTransform() { return this->m_lastTransformation; };
	virtual void setTypeOfAbilityUsed(const int ability) { this->m_typeOfAbilityEffect.x = ability; };
	virtual DirectX::XMINT4 getTypeOfAbilityUsed() const { return this->m_typeOfAbilityEffect; };
	ID3D11Buffer * GetAnimatedVertex();
	ID3D11UnorderedAccessView * GetUAV();
	void DontCallMe();

	virtual void CastShadows(const bool & shadows);
	virtual const bool & getCastShadows() const;
	
private:
	virtual void _setStaticBuffer();
	virtual void _setDynamicBuffer();

};

