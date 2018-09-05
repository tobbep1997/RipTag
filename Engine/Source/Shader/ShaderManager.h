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

	const uint8_t hashSize = 13;
	class ShaderManager
	{
	private:		
		std::vector<Shader*> shadersHashTable[hashSize];

	public:
		ShaderManager();
		~ShaderManager();
		//<ID3D11[type]Shader> 
		//Do not delete pointer. It's a managed pointer that will be deleted with  the class.
		template <typename T> T* LoadShader(const std::wstring path, const std::string entryPoint = "main");

		//Releases and deletes all shaders
		void Release();

	private:
		//Standard Hash table functions
		Shader* find(std::wstring path);
		unsigned int getKey(std::wstring path);
				
		template <typename T> T* loadShader(const std::wstring path, const std::string entryPoint);
	};


	//-------------------------------------------------------------------------------------------	
	/*
	Default PRIVATE load shader function
	This creates a new shader if i dosen't exist.
	Otherwise it just returns the existing shader
	*/
	template<typename T>
	inline T * ShaderManager::loadShader(const std::wstring path, const std::string entryPoint)
	{
		Shader * shader = this->find(path);
		if (shader)
		{
			return shader->getShader<T>();
		}
		else
		{
			shader = new Shader();
			shader->LoadShader<T>(path, entryPoint);
			shader->setKey(getKey(path));
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
	//-------------------
	/*
	Explicit template functions to know what kind of shader to create
	These calls the private function
	*/
	template<>
	inline ID3D11VertexShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11VertexShader>(path, entryPoint);
	}

	template<>
	inline ID3D11DomainShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11DomainShader>(path, entryPoint);
	}	
	
	template<>
	inline ID3D11HullShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11HullShader>(path, entryPoint);
	}	
		
	template<>
	inline ID3D11GeometryShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11GeometryShader>(path, entryPoint);
	}
			
	template<>
	inline ID3D11PixelShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11PixelShader>(path, entryPoint);
	}

	template<>
	inline ID3D11ComputeShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11ComputeShader>(path, entryPoint);
	}
	//-------------------------------------------------------------------------------------------

}