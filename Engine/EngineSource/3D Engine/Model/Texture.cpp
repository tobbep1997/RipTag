#include "EnginePCH.h"
#include "Texture.h"


Texture::Texture()
{
}



Texture::Texture(const wchar_t* file)
{
}

HRESULT Texture::Load(const wchar_t * file, bool staticTexture)
{
	std::wstring albedoName = file;
	std::wstring normalName = file;
	std::wstring ORMname = file;


	albedoName.append(L"_ALBEDO.png");
	normalName.append(L"_NORMAL.png");
	ORMname.append(L"_ORM.png");




	HRESULT hr = DirectX::CreateWICTextureFromFile(DX::g_device, albedoName.c_str(), &m_texture[0], &m_SRV[0]);
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
	
	hr = DirectX::CreateWICTextureFromFile(DX::g_device, normalName.c_str(), &m_texture[1], &m_SRV[1]);
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


	hr = DirectX::CreateWICTextureFromFile(DX::g_device, ORMname.c_str(), &m_texture[2], &m_SRV[2]);
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
