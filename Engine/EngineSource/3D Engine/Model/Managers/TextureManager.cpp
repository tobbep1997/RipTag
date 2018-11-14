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
}

void TextureManager::loadTextures(const std::string & path)
{
	Texture* tempTexture = new Texture();
	std::wstring fullPath = this->_getFullPath(path);
	unsigned int key = this->_getKey(fullPath);

	if (m_textures[key].size() == 0)
	{
		tempTexture->setName(fullPath);
		
		tempTexture->Load(fullPath.c_str());

		m_textureMutex.lock();
		m_textures[key].push_back(tempTexture);
		m_textureMutex.unlock();
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
			
			tempTexture->Load(fullPath.c_str());
			
			m_textureMutex.lock();
			m_textures[key].push_back(tempTexture);
			m_textureMutex.unlock();
		}
		else
		{
			std::cout << yellow << path << " Texture Already loaded" << std::endl;
			std::cout << white;
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
	for (int i = 0; i < 8; i++)
	{
		std::cout << red << std::string(fullPath.begin(), fullPath.end()) << " NOT FOUND" << std::endl;
	}
	throw std::exception("TEXTURE NOT FOUND");
}

Texture * TextureManager::getSingleTextureByName(const std::wstring & name)
{
	unsigned int key = this->_getKey(name);
	if (m_textures[key][0]->getName() == name)
		return m_textures[key][0];
	else
	{
		for (int i = 0; i < 8; i++)
		{
			std::cout << red << std::string(name.begin(), name.end()) << " NOT FOUND" << std::endl;
		}
		throw std::exception("TEXTURE NOT FOUND");
	}
		
	return nullptr;
}

void TextureManager::loadSingleTexture(const std::wstring name, const std::wstring & full_path)
{
	Texture * tempTexture = new Texture();
	unsigned int key = this->_getKey(name);

	if (m_textures[key].size() == 0)
	{
		tempTexture->setName(name);

		tempTexture->LoadSingleTexture(full_path.c_str());

		m_textureMutex.lock();
		m_textures[key].push_back(tempTexture);
		m_textureMutex.unlock();
	}
	else
	{
		std::cout << yellow << std::string(name.begin(), name.end()) << " Texture Already loaded" << std::endl;
		std::cout << white;
		delete tempTexture;
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

	return true;
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
