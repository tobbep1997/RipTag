#pragma once
#include <d3d11.h>
#include <stdint.h>

class Texture
{
public:
	Texture();
	Texture(const wchar_t* file);
	~Texture();

	HRESULT Load(const wchar_t* file);
	void Bind(const uint8_t slot);
private:
	ID3D11ShaderResourceView* m_SRV = nullptr;
};

