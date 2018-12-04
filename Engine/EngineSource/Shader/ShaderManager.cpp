#include "EnginePCH.h"
#include "ShaderManager.h"

#define wendl std::wstring(L"\n");

namespace Shaders
{
	ShaderManager::ShaderManager()
	{
		nrOfShaders = 0;
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
			nrOfShaders++;

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
#if _DEBUG
		system("cls");
		std::cout << "Shader Reloader-----------------------------------------------" << std::endl;
#elif !_DEPLOY
		OutputDebugStringW(LPCWSTR(L"Shader Reloader-----------------------------------------------\n"));
#endif
		int counter = 0;
		for (int i = 0; i < m_hashSize; i++)
		{
			for (int j = 0; j < m_shadersHashTable[i].size(); j++)
			{
				counter++;
#if _DEBUG
				std::wcout << L"Processing : " << this->_getName(m_shadersHashTable[i][j]->getPath()).c_str() << std::endl;
#elif !_DEPLOY
				std::wstring s = std::wstring(L"Processing : ") + this->_getName(m_shadersHashTable[i][j]->getPath()).c_str() + wendl;
				OutputDebugStringW(LPCWSTR(s.c_str()));
#endif
				HRESULT hr = m_shadersHashTable[i][j]->ReloadShader();

				if (FAILED(hr))
				{
					_com_error err(hr);
#if _DEBUG
					std::wcout << L"Error : " << this->_getName(m_shadersHashTable[i][j]->getPath()).c_str() << std::endl;
					std::cout << "\t\t\t\t\t\t" + std::to_string(static_cast<int>((static_cast<float>(counter) / nrOfShaders) * 100)) << "%\t\tFAILED " << std::endl;
#elif !_DEPLOY
					std::wstring s = std::wstring(L"Error : ") + this->_getName(m_shadersHashTable[i][j]->getPath()).c_str() + wendl;
					OutputDebugStringW(LPCWSTR(s.c_str()));
					s = L"\t\t\t\t\t\t\t\t\t\t\t\t" + std::to_wstring(static_cast<int>((static_cast<float>(counter) / nrOfShaders) * 100)) + L"%\t\tFAILED " + wendl;
					OutputDebugStringW(LPCWSTR(s.c_str()));
#endif

				}
				else
				{
#if _DEBUG
					std::wcout << L"Complete : " << this->_getName(m_shadersHashTable[i][j]->getPath()).c_str() << std::endl;
					std::cout << "\t\t\t\t\t\t" + std::to_string(static_cast<int>((static_cast<float>(counter) / nrOfShaders) * 100)) << "%\t\tDone " << std::endl;
#elif !_DEPLOY
					std::wstring s = std::wstring(L"Complete : ") + this->_getName(m_shadersHashTable[i][j]->getPath()).c_str() + wendl;
					OutputDebugStringW(LPCWSTR(s.c_str()));
					s = L"\t\t\t\t\t\t\t\t\t\t\t\t" + std::to_wstring(static_cast<int>((static_cast<float>(counter) / nrOfShaders) * 100)) + L"%\t\tDone " + wendl;
					OutputDebugStringW(LPCWSTR(s.c_str()));
#endif
				}

		
			}
		}
#if _DEBUG
		std::cout << "All done :)---------------------------------------------------" << std::endl;
#elif !_DEPLOY
		OutputDebugStringW(LPCWSTR(L"All done :)---------------------------------------------------\n"));
#endif
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
				nrOfShaders--;
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
		m_shadersHashTable->clear();
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

	std::wstring ShaderManager::_getName(const std::wstring & path)
	{		
		size_t t = path.find_last_of(L'/');
		return path.substr(t + 1, path.size());
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