#pragma once
#include <DirectXMath.h>
class Transform2D
{
private:
	DirectX::XMFLOAT2A m_position;
	DirectX::XMFLOAT2A m_size;

	DirectX::XMFLOAT4X4A m_worldMatrix;
public:
	Transform2D();
	~Transform2D();

	virtual void setPosition(const float & x, const float & y);
	virtual void setPosition(const DirectX::XMFLOAT2A & pos);

	virtual const DirectX::XMFLOAT2A & getPosition() const;

	virtual void setScale(const float & width, const float & height);
	virtual void setScale(const DirectX::XMFLOAT2A & size);

	virtual DirectX::XMFLOAT2 getScale() const;

	virtual const DirectX::XMFLOAT2A & getSize() const;
	virtual const DirectX::XMFLOAT4X4A & getWorldMatrix();

};

