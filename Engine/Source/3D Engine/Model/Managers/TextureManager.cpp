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

	tempTexture->setName(fullPath);
	tempTexture->Load(fullPath.c_str());

	m_textures[key].push_back(tempTexture);
	
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
