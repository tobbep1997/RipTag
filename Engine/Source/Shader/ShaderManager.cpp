#include "ShaderManager.h"

namespace Shaders
{
	/*
	void ShaderManager::loadShader(ShaderType type, const std::wstring path, const std::string entryPoint)
	{
		if (!this->find(path))
		{
			Shader * shader = new Shader();
			shader->LoadShader(type, path, entryPoint);
			shader->setKey(getKey(path));
			shaders[shader->getKey()].push_back(shader);
		}
	}
	*/

	void ShaderManager::Release()
	{
		for (unsigned int i = 0; i < hashSize; i++)
		{
			for (unsigned int j = 0; j < shaders[i].size(); j++)
			{
				shaders[i][j]->Release();
				delete shaders[i][j];
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
		for (int i = 0; i < shaders[key].size(); i++)
		{
			if (shaders[key][i]->getPath() == path)
				return shaders[key][i];
		}
		return nullptr;
	}


}