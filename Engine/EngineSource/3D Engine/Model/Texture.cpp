#include "EnginePCH.h"
#include "Texture.h"
#include "2D Engine/DirectXTK/DDSTextureLoader.h"
#include "Source/Timer/DeltaTime.h"


Texture::Texture()
{
}



Texture::Texture(const wchar_t* file)
{
}

HRESULT Texture::Load(const wchar_t * file, bool staticTexture, const std::string & extension)
{
	std::wstring albedoName = file;
	std::wstring normalName = file;
	std::wstring ORMname = file;

	std::wstring setting;
	size_t maxTextureSize = 512;

	if (extension == ".DDS")
	{
		switch (SettingLoader::g_windowContext->graphicsQuality)
		{
		case 0:
			setting = L"_256";
			maxTextureSize = 256;
			break;
		case 1:
			setting = L"_512";
			maxTextureSize = 512;
			break;
		case 2:
			setting = L"_1024";
			maxTextureSize = 1024;
			break;
		case 3:
			setting = L"_2048";
			maxTextureSize = 2048;
			break;
		default:
			setting = L"_128";
			maxTextureSize = 128;
			break;
		}

	}


	albedoName.append(L"_ALBEDO");
	albedoName.append(setting);
	albedoName.append(std::wstring(extension.begin(), extension.end()));

	normalName.append(L"_NORMAL");
	normalName.append(setting);
	normalName.append(std::wstring(extension.begin(), extension.end()));

	ORMname.append(L"_ORM");
	ORMname.append(setting);
	ORMname.append(std::wstring(extension.begin(), extension.end()));




	HRESULT hr;
	if (extension != ".png")
		hr = DirectX::CreateDDSTextureFromFile(DX::g_device, DX::g_deviceContext, albedoName.c_str(), &m_texture[0], &m_SRV[0], maxTextureSize);
	else
		hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, albedoName.c_str(), &m_texture[0], &m_SRV[0], maxTextureSize);
	if (FAILED(hr))
	{
		std::string p = std::string(albedoName.begin(), albedoName.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}
	if (!staticTexture)
	{
		m_texture[0]->Release();
		m_texture[0] = nullptr;
	}
	
	if (extension != ".png")
		hr = DirectX::CreateDDSTextureFromFile(DX::g_device, DX::g_deviceContext, normalName.c_str(), &m_texture[1], &m_SRV[1], maxTextureSize);
	else
		hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, normalName.c_str(), &m_texture[1], &m_SRV[1], maxTextureSize);
	if (FAILED(hr))
	{
		std::string p = std::string(normalName.begin(), normalName.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}
	if (!staticTexture)
	{
		m_texture[1]->Release();
		m_texture[1] = nullptr;
	}


	if (extension != ".png")
		hr = DirectX::CreateDDSTextureFromFile(DX::g_device, DX::g_deviceContext, ORMname.c_str(), &m_texture[2], &m_SRV[2], maxTextureSize);
	else
		hr = DirectX::CreateWICTextureFromFile(DX::g_device, DX::g_deviceContext, ORMname.c_str(), &m_texture[2], &m_SRV[2], maxTextureSize);
	if (FAILED(hr))
	{
		std::string p = std::string(ORMname.begin(), ORMname.end());
		size_t t = p.find_last_of(L'/');
		p = p.substr(t + 1);
		std::cout << red << p << " Failed to Load" << std::endl;
		std::cout << white;
		return hr;
	}
	if (!staticTexture)
	{
		m_texture[2]->Release();
		m_texture[2] = nullptr;
	}
	return hr;
}

HRESULT Texture::LoadSingleTexture(const wchar_t * absolutePath)
{
	std::wstring file = absolutePath;

	HRESULT hr;
	hr = DirectX::CreateDDSTextureFromFile(DX::g_device, file.c_str(), nullptr, &m_SRV[0]);
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
	if (index == -1)
		DX::g_deviceContext->PSSetShaderResources(slot, 3, m_SRV);
}

void Texture::setIndex(const int& index)
{
	this->index = index;
}

const int& Texture::getIndex() const
{
	return this->index;
}

void Texture::TexUnload()
{
	for (int i = 0; i < 3; i++)
	{	
		DX::SafeRelease(m_texture[i]);
		DX::SafeRelease(m_SRV[i]);
	}
}


void Texture::UnloadTexture()
{
	for (int i = 0; i < 3; i++)
	{
		if (index != -1)
			DX::SafeRelease(m_texture[i]);
		DX::SafeRelease(m_SRV[i]);
	}
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
	if (index == -1)
		UnloadTexture();
}
