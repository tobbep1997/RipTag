#pragma once
#include "Shader.h"
#include <vector>

///<summery>
/*
Creates and store shaders.
*/
///</summery>
namespace Shaders
{

	const uint8_t m_hashSize = 13;
	class ShaderManager
	{
	private:		

		std::vector<Shader*> shadersHashTable[m_hashSize];
	public:
		ShaderManager();
		~ShaderManager();
		//<ID3D11[type]Shader> 
		//Do not delete pointer. It's a managed pointer that will be deleted with  the class.
		template <typename T> T* LoadShader(const std::wstring path, const std::string entryPoint = "main");
		//DOSE NOT WORK?!?!??!
		template <typename T> T* GetShader(const std::wstring path, const std::string entryPoint = "main");

		ID3D11VertexShader * VertexInputLayout(const std::wstring path, const std::string entryPoint, D3D11_INPUT_ELEMENT_DESC inputDesc[], unsigned int size);
		ID3D11InputLayout * GetInputLayout(const std::wstring path);


		void UnloadShader(const std::wstring path);
		//Releases and deletes all shaders
		void Release();

	private:
		//Standard Hash table functions
		Shader* _find(std::wstring path);
		unsigned int _getKey(std::wstring path);
				
		template <typename T> T* _loadShader(const std::wstring path, const std::string entryPoint);
	};


	//-------------------------------------------------------------------------------------------	
	/*
	Default PRIVATE load shader function
	This creates a new shader if i dosen't exist.
	Otherwise it just returns the existing shader
	*/
	template<typename T>
	inline T * ShaderManager::_loadShader(const std::wstring path, const std::string entryPoint)
	{
		Shader * shader = this->_find(path);
		if (shader)
		{
			return shader->getShader<T>();
		}
		else
		{
			shader = new Shader();
			shader->LoadShader<T>(path, entryPoint);
			shader->setKey(_getKey(path));
			shader->setPath(path);
			shadersHashTable[shader->getKey()].push_back(shader);

			return shader->getShader<T>();
		}
			
	}
	//-------------------------------------------------------------------------------------------
	/*
	Default public load shader function
	This dosen't do anything... It's just needed to create the other ones ._.
	*/
	template<typename T>
	inline T* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return nullptr;
	}
	
	template<typename T>
	inline T * ShaderManager::GetShader(const std::wstring path, const std::string entryPoint)
	{
		return _loadShader<T>(path, entryPoint);
	}
	
	//-------------------
	/*
	Explicit template functions to know what kind of shader to create
	These calls the private function
	*/
	template<>
	inline ID3D11VertexShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->_loadShader<ID3D11VertexShader>(path, entryPoint);
	}

	template<>
	inline ID3D11DomainShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->_loadShader<ID3D11DomainShader>(path, entryPoint);
	}	
	
	template<>
	inline ID3D11HullShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->_loadShader<ID3D11HullShader>(path, entryPoint);
	}	
		
	template<>
	inline ID3D11GeometryShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->_loadShader<ID3D11GeometryShader>(path, entryPoint);
	}
			
	template<>
	inline ID3D11PixelShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->_loadShader<ID3D11PixelShader>(path, entryPoint);
	}

	template<>
	inline ID3D11ComputeShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->_loadShader<ID3D11ComputeShader>(path, entryPoint);
	}
	//-------------------------------------------------------------------------------------------

}