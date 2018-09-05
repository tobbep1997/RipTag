#pragma once
#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <debugapi.h>
#include <comdef.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include "../ShaderCreator.h"
#include "../3D Engine/Extern.h"

namespace Shaders
{

	enum ShaderType
	{
		Vertex,
		domain,
		Hull,
		Geometry,
		Pixel,
		Compute
	};


	class Shader
	{
	private:

		ID3D11VertexShader		* m_vertexShader;
		ID3D11DomainShader		* m_domainShader;
		ID3D11HullShader		* m_hullShader;
		ID3D11GeometryShader	* m_geometryShader;
		ID3D11PixelShader		* m_pixelShader;
		ID3D11ComputeShader		* m_computeShader;
			   
		std::wstring m_path;
		unsigned int m_key;
		ShaderType m_type;

		
	public:
		Shader();
		~Shader();

		void setPath(const std::wstring & path);
		void setKey(const unsigned int key);
		void setType(const ShaderType & shaderType);

		std::wstring getPath() const;
		unsigned int getKey() const;
		ShaderType getType() const;

		//Load shaders
		template <typename T> void LoadShader(const std::wstring path, const std::string entryPoint = "main");
		template <> void LoadShader<ID3D11VertexShader>(const std::wstring path, const std::string entryPoint);
		template <> void LoadShader<ID3D11PixelShader>(const std::wstring path, const std::string entryPoint);
		
		//Get Shaders
		template <typename T> T* getShader() const {
			return nullptr;
		}
		template <> ID3D11VertexShader * getShader<ID3D11VertexShader>() const {
			return this->m_vertexShader;
		}
		template <> ID3D11PixelShader * getShader<ID3D11PixelShader>() const {
			return this->m_pixelShader;
		}		
		void Release();

	};
	template<>
	inline void Shader::LoadShader<ID3D11VertexShader>(const std::wstring path, const std::string entryPoint)
	{
		ShaderCreator::CreateVertexShader(DX::g_device, m_vertexShader, path.c_str(), entryPoint.c_str());
	}

	template<>
	inline void Shader::LoadShader<ID3D11PixelShader>(const std::wstring path, const std::string entryPoint)
	{
		ShaderCreator::CreatePixelShader(DX::g_device, m_pixelShader, path.c_str(), entryPoint.c_str());
	}

	

}
