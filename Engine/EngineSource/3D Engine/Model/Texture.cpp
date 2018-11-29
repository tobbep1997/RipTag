#include "EnginePCH.h"
#include "Texture.h"


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
	std::wstring albedoName = file;
	std::wstring normalName = file;
	std::wstring ORMname = file;


	albedoName.append(L"_ALBEDO.png");
	ORMname.append(L"_ORM.png");
	normalName.append(L"_NORMAL.png");




	HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, albedoName.c_str(), nullptr, &m_SRV[0]);
	if (FAILED(hr))
	{ 
		std::string p = std::string(albedoName.begin(), albedoName.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}
	
	hr = DirectX::CreateWICTextureFromFile(DX::g_device, normalName.c_str(), nullptr, &m_SRV[1]);
	if (FAILED(hr))
	{
		std::string p = std::string(normalName.begin(), normalName.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}

	hr = DirectX::CreateWICTextureFromFile(DX::g_device, ORMname.c_str(), nullptr, &m_SRV[2]);
	if (FAILED(hr))
	{
		std::string p = std::string(ORMname.begin(), ORMname.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}

	/*hr = DirectX::CreateWICTextureFromFile(DX::g_device,DX::g_deviceContext, albedoName.c_str(), nullptr, &m_SRV[0]);
	hr = DirectX::CreateWICTextureFromFile(DX::g_device, normalName.c_str(), nullptr, &m_SRV[1]);
	hr = DirectX::CreateWICTextureFromFile(DX::g_device, ORMname.c_str(), nullptr, &m_SRV[2]);*/
	return hr;
}

HRESULT Texture::LoadSingleTexture(const wchar_t * absolutePath)
{
	std::wstring file = absolutePath;

	HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, file.c_str(), nullptr, &m_SRV[0]);
	if (FAILED(hr))
	{
		std::string p = std::string(file.begin(), file.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}
	return E_FAIL;
}

void Texture::Bind(const uint8_t slot)
{
	DX::g_deviceContext->PSSetShaderResources(slot, 3, m_SRV);
}

void Texture::setName(const std::wstring & name)
{
	this->m_textureName = name;
}

const std::wstring & Texture::getName() const
{
	return this->m_textureName;
}

Texture::~Texture()
{
	for (int i = 0; i < 3; i++)
	{
		DX::SafeRelease(m_SRV[i]);
	}	
}
