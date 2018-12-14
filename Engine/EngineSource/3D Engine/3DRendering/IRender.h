#pragma once
#include <vector>
class ForwardRender;
class Camera;
class IRender
{
protected:
	IRender();
	ForwardRender * p_forwardRender;
public:
	virtual ~IRender();

	virtual HRESULT Init(ForwardRender * forwardRender = nullptr) = 0;
	virtual HRESULT Release() = 0;

	virtual HRESULT Draw() = 0;
	virtual HRESULT Draw(Drawable * drawable, const Camera & camera) = 0;
	virtual HRESULT Draw(std::vector<DX::INSTANCING::GROUP> group, const Camera & camera) = 0;
	

};

