#pragma once
#include <d3d11.h>
#include <stdint.h>
#include <string>
class Texture
{
public:
	Texture();
	Texture(const wchar_t* file);
	~Texture();

	HRESULT Load(const wchar_t* file);
	void Bind(const uint8_t slot);
	ID3D11ShaderResourceView* m_SRV[3] = { nullptr, nullptr, nullptr };
	std::string textureName;
private:
};

