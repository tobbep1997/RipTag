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

	unsigned int				m_currentTexture = 0;
	unsigned int				m_static_textures = 0;

	ID3D11Texture2D			 *	m_static_TEX;
	ID3D11ShaderResourceView *	m_static_SRV;

public:
	TextureManager();
	~TextureManager();
	void Init();
	void loadTextures(const std::string & path, bool m_static_texture = false);
	Texture* getTexture(const std::string & path);
	Texture* getGUITextureByName(const std::wstring& name);


	void loadGUITexture(const std::wstring name, const std::wstring & full_path);

	bool UnloadTexture(const std::string & path);
	bool UnloadAllTexture();
	bool UnloadGUITextures();

	const unsigned int getLoadedTextures() const;
private:
	void MapStaticTextures();
	unsigned int _getKey(const std::wstring & path);
	std::wstring _getFullPath(const std::string & name);
};

