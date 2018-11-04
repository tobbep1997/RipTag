#pragma once
#include <vector>
#include <mutex>

class Texture;

const unsigned int TEXTURE_HASHTABLE_SIZE = 13;
class TextureManager
{
private:
	std::mutex m_textureMutex;

	std::vector<Texture*> m_textures[TEXTURE_HASHTABLE_SIZE];
	
public:
	TextureManager();
	~TextureManager();
	void loadTextures(const std::string & path);
	Texture* getTexture(const std::string & path);

	bool UnloadTexture(const std::string & path);
private:
	unsigned int _getKey(const std::wstring & path);
	std::wstring _getFullPath(const std::string & name);
};

