#pragma once
#include <vector>
#include "Texture.h"
class TextureManager
{
private: 
	std::vector<Texture*> m_Textures;
	
public:
	TextureManager();
	~TextureManager();
	void loadTextures(const std::string & path);
	Texture* getTexture(int which);

};

