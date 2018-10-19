#pragma once
#include "Transform2D.h"
class Button 
{
private:
	Transform2D * m_transform2D;
public:
	Button(Transform2D * transform2D);
	~Button();

	virtual const bool Inside(const DirectX::XMFLOAT2 & mousePos);
};

