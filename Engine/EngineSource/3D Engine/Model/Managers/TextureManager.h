#pragma once
#include <vector>
#include <mutex>

class Texture;

const unsigned int TEXTURE_HASHTABLE_SIZE = 13;
const unsigned int MAX_STATIC_TEXTURES = 35;
class TextureManager
{
private:
	std::mutex m_textureMutex;

	std::vector<Texture*> m_textures[TEXTURE_HASHTABLE_SIZE];
	std::vector<Texture*> m_GuiTextures;
	std::vector<Texture *>m_DDSTextures;

	size_t maxTextureSize;

public:
	TextureManager();
	~TextureManager();

	void loadTextures(const std::string & path, bool m_static_texture = false);
	Texture* getTexture(const std::string & path);
	Texture* getGUITextureByName(const std::wstring& name);
	Texture * getDDSTextureByName(const std::wstring & name);

	void loadGUITexture(const std::wstring name, const std::wstring & full_path);
	void loadDDSTexture(const std::wstring name, const std::wstring & full_path, const std::wstring & type);

	bool UnloadTexture(const std::string & path);
	bool UnloadAllTexture();
	bool UnloadGUITextures();
	bool UnloadDDSTextures();

	const unsigned int getLoadedTextures() const;
private:
	unsigned int _getKey(const std::wstring & path);
	std::wstring _getFullPath(const std::string & name);
};

