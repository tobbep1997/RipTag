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
	DX::SafeRelease(m_SRV);
	HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, file, nullptr, &m_SRV);
	return hr;
}

Texture::~Texture()
{
	DX::SafeRelease(m_SRV);
}
