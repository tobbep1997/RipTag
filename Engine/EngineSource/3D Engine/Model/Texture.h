#pragma once
#include <d3d11.h>
#include <stdint.h>
#include <string>

class Texture
{
private:
	int index = -1;
	std::wstring m_textureName;
public:
	Texture();
	Texture(const wchar_t* file);
	~Texture();

	HRESULT Load(const wchar_t* file, bool staticTexture = false, const std::string & extension = ".DDS");
	HRESULT LoadSingleTexture(const wchar_t * absolutePath);
	void Bind(const uint8_t slot);
	ID3D11Resource * m_texture[3] = {nullptr, nullptr, nullptr}; //DIFFUSE, NORMAL, ORM
	ID3D11ShaderResourceView* m_SRV[3] = { nullptr, nullptr, nullptr };

	void setIndex(const int & index);
	const int & getIndex() const;

	void TexUnload();
	void UnloadTexture();

	void setName(const std::wstring & name);
	const std::wstring & getName() const;
};

