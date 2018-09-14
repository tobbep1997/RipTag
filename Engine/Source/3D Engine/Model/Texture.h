#pragma once
#include <d3d11.h>

class Texture
{
public:
	Texture();
	Texture(const wchar_t* file);
	HRESULT Load(const wchar_t* file);
	~Texture();
private:
	ID3D11ShaderResourceView* m_SRV = nullptr;
};

