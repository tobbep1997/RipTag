#include "Texture.h"
#include "..\WICTextureLoader\WICTextureLoader.h"
#include "..\Extern.h"


Texture::Texture()
{
	//DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, L"poop",
}



Texture::Texture(const wchar_t* file)
{
//	DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, file, nullptr, &m_SRV);
}

HRESULT Texture::Load(const wchar_t * file)
{
	/*std::wstring tempWstring = file;
	std::wstring albedoName = L"../Assets/";*/
	std::wstring albedoName = file;
	std::wstring normalName = file;
	std::wstring ORMname = file;


	albedoName.append(L"_ALBEDO.png");
	ORMname.append(L"_ORM.png");
	normalName.append(L"_NORMAL.png");
	//DX::SafeRelease(m_SRV);
	HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, albedoName.c_str(), nullptr, &m_SRV[0]);
////m_SRV[0] = &tempSRV[0];
	hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, normalName.c_str(), nullptr, &m_SRV[1]);
////m_SRV[1] = tempSRV;
	hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, ORMname.c_str(), nullptr, &m_SRV[2]);
////m_SRV[2] = tempSRV;
	
	return hr;
}

void Texture::Bind(const uint8_t slot)
{
	DX::g_deviceContext->PSSetShaderResources(slot, 3, m_SRV);
	//DX::g_deviceContext->PSSetShaderResources(slot+1, 1, m_SRV);
	//
	//DX::g_deviceContext->PSSetShaderResources(slot+2, 1, m_SRV);

}

Texture::~Texture()
{
	for (int i = 0; i < 3; i++)
	{
		DX::SafeRelease(m_SRV[i]);
	}
	
}
