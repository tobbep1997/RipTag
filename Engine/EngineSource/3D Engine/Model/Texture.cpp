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

	std::wstring names[3] = { albedoName, normalName, ORMname };

	for (int i = 0; i < 3; i++)
	{
		/*
		 * COPYRIGHT (C) Stefan Petersson 2018
		 */
		HRESULT hr;
		ID3D11Resource* tmpResCPU = nullptr;
		hr = DirectX::CreateDDSTextureFromFileEx(DX::g_device, DX::g_deviceContext, names[i].c_str(), maxTextureSize,
			D3D11_USAGE_STAGING, 0, D3D11_CPU_ACCESS_READ, 0, false, &tmpResCPU, nullptr, nullptr);

		if (SUCCEEDED(hr))
		{
			ID3D11Texture2D* tmpTexCPU = nullptr;
			if (SUCCEEDED(hr = tmpResCPU->QueryInterface(IID_PPV_ARGS(&tmpTexCPU))))
			{
				D3D11_MAPPED_SUBRESOURCE ms;
				if (SUCCEEDED(hr = DX::g_deviceContext->Map(tmpTexCPU, 0, D3D11_MAP_READ, 0, &ms)))
				{

					D3D11_TEXTURE2D_DESC textureDesc{};
					tmpTexCPU->GetDesc(&textureDesc);
					textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
					textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					textureDesc.CPUAccessFlags = 0;
					textureDesc.MiscFlags = 0;

					D3D11_SUBRESOURCE_DATA rd{};
					rd.SysMemPitch = ms.RowPitch;
					rd.pSysMem = ms.pData;

					ID3D11Texture2D* texGPU = nullptr;
					if (SUCCEEDED(hr = DX::g_device->CreateTexture2D(&textureDesc, &rd, &texGPU)))
					{
						D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
						srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
						srv_desc.Format = textureDesc.Format;
						srv_desc.Texture2D.MipLevels = textureDesc.MipLevels;
						srv_desc.Texture2D.MostDetailedMip = 0;

						if (SUCCEEDED(hr = DX::g_device->CreateShaderResourceView(texGPU, &srv_desc, &m_SRV[i])))
						{

						}
					}
					texGPU->Release();

					DX::g_deviceContext->Unmap(tmpTexCPU, 0);
				}

				tmpTexCPU->Release();
			}
			tmpResCPU->Release();
		}
	}

	return S_OK;	
}

HRESULT Texture::LoadSingleTexture(const wchar_t * absolutePath)
{
	std::wstring file = absolutePath;


	/*
	 * COPYRIGHT (C) Stefan Petersson 2018
	 */
	HRESULT hr;
	ID3D11Resource* tmpResCPU = nullptr;
	hr = DirectX::CreateDDSTextureFromFileEx(DX::g_device,
		DX::g_deviceContext,
		file.c_str(),
		0,
		D3D11_USAGE_STAGING,
		0,
		D3D11_CPU_ACCESS_READ,
		0, 
		false, 
		&tmpResCPU, 
		nullptr, 
		nullptr);

	if (SUCCEEDED(hr))
	{
		ID3D11Texture2D* tmpTexCPU = nullptr;
		if (SUCCEEDED(hr = tmpResCPU->QueryInterface(IID_PPV_ARGS(&tmpTexCPU))))
		{
			D3D11_MAPPED_SUBRESOURCE ms;
			if (SUCCEEDED(hr = DX::g_deviceContext->Map(tmpTexCPU, 0, D3D11_MAP_READ, 0, &ms)))
			{

				D3D11_TEXTURE2D_DESC textureDesc{};
				tmpTexCPU->GetDesc(&textureDesc);
				textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				textureDesc.CPUAccessFlags = 0;
				textureDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA rd{};
				rd.SysMemPitch = ms.RowPitch;
				rd.pSysMem = ms.pData;

				ID3D11Texture2D* texGPU = nullptr;
				if (SUCCEEDED(hr = DX::g_device->CreateTexture2D(&textureDesc, &rd, &texGPU)))
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
					srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srv_desc.Format = textureDesc.Format;
					srv_desc.Texture2D.MipLevels = textureDesc.MipLevels;
					srv_desc.Texture2D.MostDetailedMip = 0;

					if (SUCCEEDED(hr = DX::g_device->CreateShaderResourceView(texGPU, &srv_desc, &m_SRV[0])))
					{
						
					}
				}
				texGPU->Release();

				DX::g_deviceContext->Unmap(tmpTexCPU, 0);
			}

			tmpTexCPU->Release();
		}
		tmpResCPU->Release();
	}


	return S_OK;
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




void Texture::UnloadTexture()
{
	for (int i = 0; i < 3; i++)
	{		
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
	UnloadTexture();
}
