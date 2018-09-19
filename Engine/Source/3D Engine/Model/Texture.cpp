#include "Texture.h"
#include "..\WICTextureLoader\WICTextureLoader.h"
#include "..\Extern.h"


Texture::Texture()
{
	//DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, L"poop",
}



Texture::Texture(const wchar_t* file)
{
	DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, file, nullptr, &m_SRV);
}

HRESULT Texture::Load(const wchar_t * file)
{
	std::wstring asd = file;
	std::wstring albedoName = L"../Assets/";
	albedoName.append(asd + L"FOLDER/" + asd + L"_ALBEDO.png");
	//DX::SafeRelease(m_SRV);
	HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, albedoName.c_str(), nullptr, &m_SRV);
	return hr;
}

void Texture::Bind(const uint8_t slot)
{
	DX::g_deviceContext->PSSetShaderResources(slot, 1, &m_SRV);
}

Texture::~Texture()
{
	DX::SafeRelease(m_SRV);
}
