#pragma once
#include <DirectXMath.h>

class Transform2D;
class Button 
{
private:
	Transform2D * m_transform2D;
public:
	Button(Transform2D * transform2D);
	~Button();

	virtual const bool Inside(const DirectX::XMFLOAT2 & mousePos);
};

