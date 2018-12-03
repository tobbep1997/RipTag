#pragma once
#include <vector>
namespace DirectX
{
	class SpriteFont;
}
class FontHandler
{
private:
	struct SpriteStruct
	{
		std::wstring fullPath;
		DirectX::SpriteFont * spriteFont;
	
		SpriteStruct(std::wstring fullPath, DirectX::SpriteFont * spriteFont)
		{
			this->fullPath = fullPath;
			this->spriteFont = spriteFont;
		}

		void Release()
		{
			
			delete spriteFont;
		}
	};

private:
	//Make stuff easy for the user
	static const wchar_t * BASE_PATH;
	static const wchar_t * FILE_TYPE;
	static const unsigned short HASH_TABLE_SIZE = 13;
	static std::vector<SpriteStruct *> m_spriteTable[HASH_TABLE_SIZE];

private:
	//These are just to manage the hash table
	static const unsigned short getKey(const std::wstring & path);
	static const bool exists(const std::wstring & path, const unsigned short key);
	static std::wstring getFullPath(const std::string & name);

public:

	/*
		This is the part that the end user sees and all the end user needs
	*/


	// loadFont loads the font that is inputed by name and NOT by path
	// It must be in the $(BASE_PATH) folder
	static void loadFont(const std::string & name);
	// This returns a poiter to the requested font.
	// If i dose not exist it throws
	// DO NOT DELETE THIS POINTER
	static DirectX::SpriteFont * getFont(const std::string & name);

	//This must be called when the program exits
	//IF NOT THERE WILL BE CONSEQUENCES 
	static void Release();
};

