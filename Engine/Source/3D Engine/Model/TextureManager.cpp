#include "TextureManager.h"



TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
	for (int i = 0; i < m_Textures.size(); i++)
	{
		delete m_Textures[i];
	}
}

void TextureManager::loadTextures(const std::string & path)
{
	Texture* tempTexture = new Texture();
	tempTexture->textureName = path;
	std::wstring tempWstring;
	tempWstring.assign(path.begin(), path.end());
	std::wstring texture = L"../Assets/";
	texture.append(tempWstring + L"FOLDER/" + tempWstring);
	tempTexture->Load(texture.c_str());
	m_Textures.push_back(tempTexture);
	
}

Texture * TextureManager::getTexture(int which)
{
	return m_Textures[which];
}
