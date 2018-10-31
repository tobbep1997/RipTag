#include "TextureManager.h"
#include <iostream>


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
			//Texture already loaded
			std::wcout << "Texture " << fullPath << " Already loaded" << std::endl;
			delete tempTexture;
			//return false;
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
			std::wcout << "Texture mesh " << fullPath << " Unloaded" << std::endl;
			return true;
		}
	}
	return false;
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
