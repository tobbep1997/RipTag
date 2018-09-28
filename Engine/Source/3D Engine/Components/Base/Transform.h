#pragma once
#include <DirectXMath.h>
class Transform
{
protected:
	DirectX::XMFLOAT4A p_position;
	DirectX::XMFLOAT4A p_scale;
	DirectX::XMFLOAT4A p_rotation;

	DirectX::XMFLOAT4X4A p_worldMatrix;

	//Calculates the worldMatrix
	void p_calcWorldMatrix();
public:
	Transform();
	virtual ~Transform();

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

	virtual void addRotation(const DirectX::XMFLOAT4A & rot);
	virtual void addRotation(const float & x, const float & y, const float & z, const float & w = 0.0f);

	
	virtual const DirectX::XMFLOAT4A & getPosition() const;
	virtual const DirectX::XMFLOAT4A & getScale() const;
	virtual const DirectX::XMFLOAT4A & getEulerRotation() const;
};

