#include "ShaderManager.h"

namespace Shaders
{
	ShaderManager::ShaderManager()
	{
	}
	ShaderManager::~ShaderManager()
	{
	}
	void ShaderManager::UnloadShader(const std::wstring path)
	{
		unsigned int key = getKey(path);
		for (int i = 0; i < shadersHashTable[key].size(); i++)
		{
			if (shadersHashTable[key][i]->getPath() == path)
			{
				shadersHashTable[key][i]->Release();
				delete shadersHashTable[key][i];
				shadersHashTable[key].erase(shadersHashTable[key].begin() + i);

			}
		}
	}
	void ShaderManager::Release()
	{
		for (unsigned int i = 0; i < hashSize; i++)
		{
			for (unsigned int j = 0; j < shadersHashTable[i].size(); j++)
			{
				shadersHashTable[i][j]->Release();
				delete shadersHashTable[i][j];
			}
		}
	}

	unsigned int ShaderManager::getKey(std::wstring path)
	{
		unsigned int sum = 0;
		for (int i = 0; i < path.size(); i++)
		{
			sum += path[i];
		}
		return sum % hashSize;
	}

	Shader* ShaderManager::find(std::wstring path)
	{
		unsigned int key = getKey(path);
		for (int i = 0; i < shadersHashTable[key].size(); i++)
		{
			if (shadersHashTable[key][i]->getPath() == path)
				return shadersHashTable[key][i];
		}
		return nullptr;
	}


}