#include "EnginePCH.h"
#include "TextureManager.h"


TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
	for (unsigned int i = 0; i < TEXTURE_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_textures[i].size(); j++)
		{
			delete m_textures[i][j];
		}
	}
	this->UnloadGUITextures();
}

void TextureManager::Init()
{
		switch (SettingLoader::g_windowContext->graphicsQuality)
		{
		case 0:			
			maxTextureSize = 256;
			break;
		case 1:
			maxTextureSize = 512;
			break;
		case 2:
			maxTextureSize = 1024;
			break;
		case 3:
			maxTextureSize = 2048;
			break;
		default:
			maxTextureSize = 128;
			break;
		}

	

	DXRHC::CreateTexture2D(m_static_TEX,
		maxTextureSize,
		maxTextureSize,
		D3D11_BIND_SHADER_RESOURCE,
		1, 
		1, 
		0,
		MAX_STATIC_TEXTURES * 3U, 
		0, 
		0, 
		DXGI_FORMAT_BC3_UNORM,
		D3D11_USAGE_IMMUTABLE);

	DXRHC::CreateShaderResourceView(m_static_TEX, 
		m_static_SRV,
		0,
		DXGI_FORMAT_BC3_UNORM,
		D3D11_SRV_DIMENSION_TEXTURE2DARRAY, 
		MAX_STATIC_TEXTURES * 3U,
		0, 
		0, 
		1);
	MapStaticTextures();
}

void TextureManager::loadTextures(const std::string & path, bool m_static_texture)
{
	Texture* tempTexture = new Texture();
	std::wstring fullPath = this->_getFullPath(path);
	unsigned int key = this->_getKey(fullPath);

	if (m_textures[key].empty())
	{
		tempTexture->setName(fullPath);

		if (m_static_texture)
			tempTexture->Load(fullPath.c_str(), m_static_texture, ".DDS");
		else
			tempTexture->Load(fullPath.c_str(), m_static_texture);

		m_textureMutex.lock();
		m_textures[key].push_back(tempTexture);
		m_textureMutex.unlock();

		if(m_static_texture)
		{
			for (int i = 0; i < 3 && this->m_static_textures < MAX_STATIC_TEXTURES * 3; i++)
			{
				if (tempTexture->m_SRV[i])
				{
					//DX::g_deviceContext->CopySubresourceRegion(m_static_TEX, 
					//	this->m_static_textures,
					//	0, 
					//	0, 
					//	0, 
					//	tempTexture->m_texture[i],
					//	0, 
					//	NULL);
					D3D11_BOX destRegion;
					destRegion.left = 0;
					destRegion.right = 512 * 4 * 4 * 1;
					destRegion.top = 0;
					destRegion.bottom = 1;
					destRegion.front = 0;
					destRegion.back = 1;
					D3D11_TEXTURE2D_DESC desc;
					((ID3D11Texture2D*)tempTexture->m_texture[i])->GetDesc(&desc);
					
					UINT calcSub = D3D11CalcSubresource(0, this->m_static_textures, 1);
					DX::g_deviceContext->UpdateSubresource(m_static_TEX, calcSub, NULL, tempTexture->m_texture[i], 0, 0);
					//DX::g_deviceContext->UpdateSubresource(this->m_static_TEX, this->m_static_textures, NULL, tempTexture->m_texture[i], calcSub, calcSub);
				}

				this->m_static_textures++;
			}
			tempTexture->setIndex(this->m_currentTexture++);
			tempTexture->TexUnload();
			DX::g_deviceContext->Flush();

			
		}
	}
	else
	{
		bool duplicate = false;
		for (unsigned int i = 0; i < m_textures[key].size(); i++)
		{
			if (m_textures[key].at(i)->getName() == fullPath)
			{
				duplicate = true;
			}
		}
		if (duplicate == false)
		{
			tempTexture->setName(fullPath);
			
			if (m_static_texture)
				tempTexture->Load(fullPath.c_str(), m_static_texture, ".DDS");
			else
				tempTexture->Load(fullPath.c_str(), m_static_texture);
			
			m_textureMutex.lock();
			m_textures[key].push_back(tempTexture);
			m_textureMutex.unlock();

			if (m_static_texture)
			{
				for (int i = 0; i < 3 && this->m_static_textures < MAX_STATIC_TEXTURES * 3; i++)
				{
					if (tempTexture->m_SRV[i])
					{
						//DX::g_deviceContext->CopySubresourceRegion(m_static_TEX,
						//	this->m_static_textures,
						//	0,
						//	0,
						//	0,
						//	tempTexture->m_texture[i],
						//	0,
						//	NULL);
						D3D11_BOX destRegion;
						destRegion.left = 0;
						destRegion.right = 512 * 4*4*1;
						destRegion.top = 0;
						destRegion.bottom = 1;
						destRegion.front = 0;
						destRegion.back = 1;

						DX::g_deviceContext->UpdateSubresource(m_static_TEX, this->m_static_textures, &destRegion, tempTexture->m_texture[i], maxTextureSize, 0);

						//UINT calcSub = D3D11CalcSubresource(0, this->m_static_textures, 1);
						//DX::g_deviceContext->UpdateSubresource(this->m_static_TEX, this->m_static_textures, NULL, tempTexture->m_texture[i], calcSub, calcSub);
					}

					this->m_static_textures++;
				}
				tempTexture->setIndex(this->m_currentTexture++);
				tempTexture->TexUnload();
				DX::g_deviceContext->Flush();
				
			}
		}
		else
		{
			delete tempTexture;	
		}
	}

}

Texture * TextureManager::getTexture(const std::string & path)
{
	std::wstring fullPath = this->_getFullPath(path);
	unsigned int key = this->_getKey(fullPath);

	
	for (unsigned int i = 0; i < m_textures[key].size(); i++)
	{
		if (m_textures[key][i]->getName() == fullPath)
		{
			return m_textures[key][i];
		}
	}
	
	return nullptr;
}

Texture * TextureManager::getGUITextureByName(const std::wstring & name)
{
	for (auto &t : m_GuiTextures)
	{
		if (t->getName() == name)
			return t;
	}

	return nullptr;
}

void TextureManager::MapStaticTextures()
{
	DX::g_deviceContext->PSSetShaderResources(4, 1, &m_static_SRV);
	DX::g_deviceContext->Flush();

	m_static_SRV->Release();
	m_static_TEX->Release();

	//std::free(m_static_SRV);
	//std::free(m_static_TEX);

	//m_static_SRV = nullptr;
	//m_static_TEX = nullptr;
				 
	this->m_currentTexture = 0;
	this->m_static_textures = 0;
}

void TextureManager::loadGUITexture(const std::wstring name, const std::wstring & full_path)
{
	Texture * tempTexture = new Texture();

	if (m_GuiTextures.size() > 0)
	{
		for (auto &t : m_GuiTextures)
		{
			if (t->getName() == name)
			{
				delete tempTexture;
				return;
			}
		}
		//new entry
		tempTexture->setName(name);

		tempTexture->LoadSingleTexture(full_path.c_str());

		m_textureMutex.lock();
		m_GuiTextures.push_back(tempTexture);
		m_textureMutex.unlock();
	}
	else
	{
		//first entry is always free
		tempTexture->setName(name);

		tempTexture->LoadSingleTexture(full_path.c_str());

		m_textureMutex.lock();
		m_GuiTextures.push_back(tempTexture);
		m_textureMutex.unlock();
	}
}

bool TextureManager::UnloadTexture(const std::string& path)
{
	std::wstring fullPath = this->_getFullPath(path);
	unsigned int key = this->_getKey(fullPath);

	for (unsigned int i = 0; i < m_textures[key].size(); i++)
	{
		if (m_textures[key].at(i)->getName() == fullPath)
		{
			delete m_textures[key].at(i);
			m_textures[key].erase(m_textures[key].begin() + i);
			return true;
		}
	}
	return false;
}

bool TextureManager::UnloadAllTexture()
{
	for (unsigned int i = 0; i < TEXTURE_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_textures[i].size(); j++)
		{
			delete m_textures[i][j];
		}
		m_textures[i].clear();
	}
	this->m_currentTexture = 0;
	this->m_static_textures = 0;
	return true;
}

bool TextureManager::UnloadGUITextures()
{
	for (auto & t : m_GuiTextures)
		delete t;
	m_GuiTextures.clear();

	return false;
}

const unsigned int TextureManager::getLoadedTextures() const
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < TEXTURE_HASHTABLE_SIZE; i++)
	{
		for (unsigned int j = 0; j < m_textures[i].size(); j++)
		{
			count++;
		}		
	}
	return count;
}

unsigned int TextureManager::_getKey(const std::wstring & path)
{
	unsigned int sum = 0;
	for (unsigned int i = 0; i < path.size(); i++)
	{
		sum += path[i];
	}
	return sum % TEXTURE_HASHTABLE_SIZE;
}

std::wstring TextureManager::_getFullPath(const std::string & name)
{
	std::wstring tempWstring = std::wstring(name.begin(), name.end());	
	std::wstring texture = L"../Assets/";
	texture.append(tempWstring + L"FOLDER/" + tempWstring);
	return texture;
}
