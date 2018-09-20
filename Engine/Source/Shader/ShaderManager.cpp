#include "ShaderManager.h"

#if _DEBUG
#include <iostream>
#endif

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
		Shader * shader = this->_find(path);
		if (shader)
		{
			return shader->getShader<ID3D11VertexShader>();
		}
		else
		{
			shader = new Shader();		
			
			shader->VertexInputLayout(path, entryPoint, inputDesc, size);
			shader->setKey(_getKey(path));
			shader->setPath(path);
			shader->setType(Shaders::ShaderType::Vertex);
			shader->setEntryPoint(entryPoint);
			
			m_shadersHashTable[shader->getKey()].push_back(shader);

			return shader->getShader<ID3D11VertexShader>();
		}
	}
	ID3D11InputLayout * ShaderManager::GetInputLayout(const std::wstring path)
	{
		unsigned int key = _getKey(path);
		for (unsigned int i = 0; i < m_shadersHashTable[key].size(); i++)
		{
			if (m_shadersHashTable[key][i]->getPath() == path)
				return m_shadersHashTable[key][i]->getInputLayout();
		}
		return nullptr;
	}
	void ShaderManager::ReloadAllShaders()
	{
		struct shaderStruct
		{
			std::wstring path;
			std::string entryPoint;
			Shaders::ShaderType type;
		};
		std::vector<shaderStruct> shaderStructs;
		int count = 0;
		for (int i = 0; i < m_hashSize; i++)
		{
			for (int j = 0; j < m_shadersHashTable[i].size(); j++)
			{
				if (m_shadersHashTable[i][j]->getType() == Shaders::Vertex)
					continue;
				shaderStruct s;
				s.type = m_shadersHashTable[i][j]->getType();
				s.path = m_shadersHashTable[i][j]->getPath();
				s.entryPoint = m_shadersHashTable[i][j]->getEntryPoint();
				shaderStructs.push_back(s);
				count++;

				m_shadersHashTable[i][j]->Release();
				delete m_shadersHashTable[i][j];
				m_shadersHashTable[i].erase(m_shadersHashTable[i].begin() + j);				
			}
		}
		for (int i = 0; i < shaderStructs.size(); i++)
		{
			switch (shaderStructs[i].type)
			{
			case Shaders::Vertex:
				_loadShader<ID3D11VertexShader>(shaderStructs[i].path, shaderStructs[i].entryPoint, shaderStructs[i].type);
				break;
			case Shaders::domain:
				_loadShader<ID3D11DomainShader>(shaderStructs[i].path, shaderStructs[i].entryPoint, shaderStructs[i].type);
				break;
			case Shaders::Hull:
				_loadShader<ID3D11HullShader>(shaderStructs[i].path, shaderStructs[i].entryPoint, shaderStructs[i].type);
				break;
			case Shaders::Geometry:
				_loadShader<ID3D11GeometryShader>(shaderStructs[i].path, shaderStructs[i].entryPoint, shaderStructs[i].type);
				break;
			case Shaders::Pixel:
				_loadShader<ID3D11PixelShader>(shaderStructs[i].path, shaderStructs[i].entryPoint, shaderStructs[i].type);
				break;
			case Shaders::Compute:
				_loadShader<ID3D11ComputeShader>(shaderStructs[i].path, shaderStructs[i].entryPoint, shaderStructs[i].type);
				break;
			default:
#if _DEBUG
				std::cout << "Shader could not reload\nBreaking program" << std::endl;
				throw;
#endif
				break;
			}
		}
	}
	void ShaderManager::UnloadShader(const std::wstring path)
	{
		unsigned int key = _getKey(path);
		for (unsigned int i = 0; i < m_shadersHashTable[key].size(); i++)
		{
			if (m_shadersHashTable[key][i]->getPath() == path)
			{
				m_shadersHashTable[key][i]->Release();
				delete m_shadersHashTable[key][i];
				m_shadersHashTable[key].erase(m_shadersHashTable[key].begin() + i);

			}
		}
	}
	void ShaderManager::Release()
	{
		for (unsigned int i = 0; i < m_hashSize; i++)
		{
			for (unsigned int j = 0; j < m_shadersHashTable[i].size(); j++)
			{
				m_shadersHashTable[i][j]->Release();
				delete m_shadersHashTable[i][j];
			}
		}
	}

	unsigned int ShaderManager::_getKey(std::wstring path)
	{
		unsigned int sum = 0;
		for (unsigned int i = 0; i < path.size(); i++)
		{
			sum += path[i];
		}
		return sum % m_hashSize;
	}

	Shader* ShaderManager::_find(std::wstring path)
	{
		unsigned int key = _getKey(path);
		for (unsigned int i = 0; i < m_shadersHashTable[key].size(); i++)
		{
			if (m_shadersHashTable[key][i]->getPath() == path)
				return m_shadersHashTable[key][i];
		}
		return nullptr;
	}


}