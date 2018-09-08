#include "ShaderManager.h"

namespace Shaders
{
	ShaderManager::ShaderManager()
	{
		
	}
	ShaderManager::~ShaderManager()
	{
	}
	ID3D11VertexShader * ShaderManager::VertexInputLayout(const std::wstring path, const std::string entryPoint, D3D11_INPUT_ELEMENT_DESC inputDesc[], unsigned int size)
	{
		Shader * shader = this->find(path);
		if (shader)
		{
			return shader->getShader<ID3D11VertexShader>();
		}
		else
		{
			shader = new Shader();		
			
			shader->VertexInputLayout(path, entryPoint, inputDesc, size);
			shader->setKey(getKey(path));
			shader->setPath(path);
			shadersHashTable[shader->getKey()].push_back(shader);

			return shader->getShader<ID3D11VertexShader>();
		}
	}
	ID3D11InputLayout * ShaderManager::GetInputLayout(const std::wstring path)
	{
		unsigned int key = getKey(path);
		for (unsigned int i = 0; i < shadersHashTable[key].size(); i++)
		{
			if (shadersHashTable[key][i]->getPath() == path)
				return shadersHashTable[key][i]->getInputLayout();
		}
		return nullptr;
	}
	void ShaderManager::UnloadShader(const std::wstring path)
	{
		unsigned int key = getKey(path);
		for (unsigned int i = 0; i < shadersHashTable[key].size(); i++)
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
		for (unsigned int i = 0; i < path.size(); i++)
		{
			sum += path[i];
		}
		return sum % hashSize;
	}

	Shader* ShaderManager::find(std::wstring path)
	{
		unsigned int key = getKey(path);
		for (unsigned int i = 0; i < shadersHashTable[key].size(); i++)
		{
			if (shadersHashTable[key][i]->getPath() == path)
				return shadersHashTable[key][i];
		}
		return nullptr;
	}


}