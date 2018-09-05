#pragma once
#include "Shader.h"
#include <vector>

#include <iostream>
namespace Shaders
{

	const uint8_t hashSize = 13;
	class ShaderManager
	{
	private:
		std::vector<Shader*> shaders[hashSize];
		
	public:
		template <typename T> T* LoadShader(const std::wstring path, const std::string entryPoint = "main");
		void Release();

	private:
		Shader* find(std::wstring path);
		unsigned int getKey(std::wstring path);
		template <typename T> T* loadShader(const std::wstring path, const std::string entryPoint);
	};


	//-------------------------------------------------------------------------------------------


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
			shaders[shader->getKey()].push_back(shader);
			return shader->getShader<T>();
		}
			
	}

	template<typename T>
	inline T* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return nullptr;
	}	

	template<>
	inline ID3D11VertexShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11VertexShader>(path, entryPoint);
	}

	template<>
	inline ID3D11PixelShader* ShaderManager::LoadShader(const std::wstring path, const std::string entryPoint)
	{
		return this->loadShader<ID3D11PixelShader>(path, entryPoint);
	}

}