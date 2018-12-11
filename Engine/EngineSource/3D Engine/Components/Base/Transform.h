#pragma once
#include <DirectXMath.h>
#include "Source/Physics/Common/Math/b3Mat33.h"



class PlayState;

class Transform
{
private:
	Transform * m_parent;
protected:
	DirectX::XMFLOAT4A p_position;
	DirectX::XMFLOAT4A p_scale;
	DirectX::XMFLOAT4A p_rotation;


	DirectX::XMMATRIX p_forcedWorld;
	DirectX::XMMATRIX m_modelTransform;
	DirectX::XMFLOAT3X3 p_physicsRotation;

	DirectX::XMFLOAT4X4A p_worldMatrix;

	void p_calcWorldMatrix();
	void p_calcWorldMatrixForOutline(const float & lenght);
	void p_calcWorldMatrixForInsideOutline(const float & lenght);
public:
	friend PlayState;
	Transform();
	virtual ~Transform();

	virtual void setParent(Transform & parent);
	virtual const Transform & getParent() const;

	virtual void setPosition(const DirectX::XMFLOAT4A & pos);
	virtual void setPosition(const float & x, const float & y, const float & z, const float & w = 1.0f);
	
	virtual void Translate(const DirectX::XMFLOAT3 & translation);
	virtual void Translate(const float & x, const float & y, const float & z);

	virtual void setScale(const DirectX::XMFLOAT4A & scale);
	virtual void setScale(const float & x = 1.0f, const float & y = 1.0f, const float & z = 1.0f, const float & w = 1.0f);
	
	virtual void addScale(const DirectX::XMFLOAT3 & scale);
	virtual void addScale(const float & x, const float & y, const float & z);

	virtual void setRotation(const DirectX::XMFLOAT4A & rot);
	virtual void setRotation(const float & x = 0.0f, const float & y = 0.0f, const float & z = 0.0f, const float & w = 1.0f);

	virtual void setModelTransform(const DirectX::XMMATRIX& matrix);

	virtual void ForceWorld(const DirectX::XMMATRIX & world);

	virtual void addRotation(const DirectX::XMFLOAT4A & rot);
	virtual void addRotation(const float & x, const float & y, const float & z, const float & w = 0.0f);

	virtual const Transform * getTransform();

	virtual const DirectX::XMFLOAT4A & getPosition() const;
	virtual const DirectX::XMFLOAT4A & getScale() const;
	virtual const DirectX::XMFLOAT4A & getEulerRotation() const;

	virtual DirectX::XMFLOAT4X4A getWorldmatrix();
	virtual DirectX::XMFLOAT4X4A getWorldMatrixForOutline(const DirectX::XMFLOAT4A & pos);
	virtual DirectX::XMFLOAT4X4A getWorldMatrixForInsideOutline(const DirectX::XMFLOAT4A & pos);

	virtual void setPhysicsRotation(const b3Mat33 & rot);

	virtual float getLenghtToObject(const DirectX::XMFLOAT4A & pos);

	// #todoREMOVE
	void setVelocity(b3Vec3 vel);
	// #todoREMOVE

	DirectX::XMFLOAT4X4A CreateNewWorldMatrix(const DirectX::XMFLOAT3 & scale);
private:
	b3Vec3 m_Velocity = { 0.0, 0.0, 0.0 };
};

