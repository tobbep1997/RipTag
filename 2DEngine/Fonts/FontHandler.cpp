#include "Engine2DPCH.h"
#include "FontHandler.h"

const wchar_t * FontHandler::FILE_TYPE = L".spritefont";
const wchar_t * FontHandler::BASE_PATH = L"../2DEngine/Fonts/SpriteFonts/";
std::vector<FontHandler::SpriteStruct *> FontHandler::m_spriteTable[HASH_TABLE_SIZE];

const unsigned short FontHandler::getKey(const std::wstring & path)
{
	unsigned short sum = 0;
	for (unsigned int i = 0; i < path.size(); i++)
		sum += path[i];
	return sum % HASH_TABLE_SIZE;
}

const bool FontHandler::exists(const std::wstring & path, const unsigned short key)
{
	bool doseExist = false;
	for (unsigned short i = 0; i < m_spriteTable[key].size() && !doseExist; i++)
		if (m_spriteTable[key][i]->fullPath == path)		
			doseExist = true;	
	return doseExist;
}

std::wstring FontHandler::getFullPath(const std::string & name)
{
	std::wstring fullPath = BASE_PATH;
	fullPath.append(std::wstring(name.begin(), name.end()));
	fullPath.append(FILE_TYPE);
	return fullPath;
}

void FontHandler::loadFont(const std::string & name)
{
	std::wstring fullPath = getFullPath(name);
	const unsigned short key = FontHandler::getKey(fullPath);

	if (!exists(fullPath, key))
	{
		try
		{
			SpriteStruct * spriteStruct = new SpriteStruct(
				fullPath,
				new DirectX::SpriteFont(DX::g_device, fullPath.data())
			);

			spriteStruct->spriteFont->SetDefaultCharacter('X');

			m_spriteTable[key].push_back(spriteStruct);
		}
		catch (std::exception e)
		{

		}
	}

}

DirectX::SpriteFont * FontHandler::getFont(const std::string & name)
{
	std::wstring fullPath = getFullPath(name);
	const unsigned short key = FontHandler::getKey(fullPath);

	for (unsigned short i = 0; i < m_spriteTable[key].size(); i++)	
		if (fullPath == m_spriteTable[key][i]->fullPath)		
			return m_spriteTable[key][i]->spriteFont;	
	
	return nullptr;
}

void FontHandler::Release()
{
	for (unsigned short i = 0; i < HASH_TABLE_SIZE; i++)
	{
		for (unsigned short j = 0; j < m_spriteTable[i].size(); j++)
		{
			m_spriteTable[i][j]->Release();
			delete m_spriteTable[i][j];
		}
	}
}

