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

	///<summery>
	/*
	Store the acual shader
	*/
	///</summery>
	class Shader
	{
	private:
		//There is no better way of doing this ._.
		ID3D11VertexShader		* m_vertexShader;
		ID3D11DomainShader		* m_domainShader;
		ID3D11HullShader		* m_hullShader;
		ID3D11GeometryShader	* m_geometryShader;
		ID3D11PixelShader		* m_pixelShader;
		ID3D11ComputeShader		* m_computeShader;
		ID3D11InputLayout		* m_inputLayout;

		

		//-------------------------------------------------------------------------------------------
		//identifier
		std::wstring m_path;
		unsigned int m_key;
		ShaderType m_type;
		std::string m_entryPoint;

		//Light quaris? light ocluion google fucking lights TODO
	public:
		Shader();
		~Shader();
		//-------------------------------------------------------------------------------------------
		//Get/Set functions
		void setPath(const std::wstring & path);
		std::wstring getPath() const;

		void setKey(const unsigned int key);
		unsigned int getKey() const;

		void setType(const ShaderType & shaderType);
		ShaderType getType() const;

		void setEntryPoint(const std::string & entryPoint);
		std::string getEntryPoint() const;
		//-------------------------------------------------------------------------------------------
		/*
		Default load shader function
		This dosen't do anything... It's just needed to create the other ones ._.
		*/
		template <typename T> HRESULT LoadShader(const std::wstring path, const std::string entryPoint = "main");
		ID3D11VertexShader * VertexInputLayout(const std::wstring path, const std::string entryPoint, D3D11_INPUT_ELEMENT_DESC inputDesc[], unsigned int size);
		void setInputLayout(ID3D11InputLayout *& inputLayout);
		ID3D11InputLayout * getInputLayout();
		/*
		Loads the correct shader
		*/
		template <> HRESULT LoadShader<ID3D11VertexShader>(const std::wstring path, const std::string entryPoint);
		//-------------------------------------------------------------------------------------------
		//Get Shaders
		template <typename T> T* getShader() const {
			return nullptr;
		}
		template <> ID3D11VertexShader * getShader<ID3D11VertexShader>() const {
			return this->m_vertexShader;
		}
		template <> ID3D11DomainShader * getShader<ID3D11DomainShader>() const {
			return this->m_domainShader;
		}
		template <> ID3D11HullShader * getShader<ID3D11HullShader>() const {
			return this->m_hullShader;
		}
		template <> ID3D11GeometryShader * getShader<ID3D11GeometryShader>() const {
			return this->m_geometryShader;
		}
		template <> ID3D11PixelShader * getShader<ID3D11PixelShader>() const {
			return this->m_pixelShader;
		}		
		template <> ID3D11ComputeShader * getShader<ID3D11ComputeShader>() const {
			return this->m_computeShader;
		}
		//-------------------------------------------------------------------------------------------
		//Relese the shader
		void Release();
		void ReleaseWithoutInputLayout();


		HRESULT ReloadShader();
	};
	template<>
	inline HRESULT Shader::LoadShader<ID3D11VertexShader>(const std::wstring path, const std::string entryPoint)
	{
		return ShaderCreator::CreateVertexShader(DX::g_device, m_vertexShader, path.c_str(), entryPoint.c_str());
	}

	template<>
	inline HRESULT Shader::LoadShader<ID3D11DomainShader>(const std::wstring path, const std::string entryPoint)
	{
		return ShaderCreator::CreateDomainShader(DX::g_device, m_domainShader, path.c_str(), entryPoint.c_str());
	}

	template<>
	inline HRESULT Shader::LoadShader<ID3D11HullShader>(const std::wstring path, const std::string entryPoint)
	{
		return ShaderCreator::CreateHullShader(DX::g_device, m_hullShader, path.c_str(), entryPoint.c_str());
	}

	template<>
	inline HRESULT Shader::LoadShader<ID3D11GeometryShader>(const std::wstring path, const std::string entryPoint)
	{
		return ShaderCreator::CreateGeometryShader(DX::g_device, m_geometryShader, path.c_str(), entryPoint.c_str());
	}

	template<>
	inline HRESULT Shader::LoadShader<ID3D11PixelShader>(const std::wstring path, const std::string entryPoint)
	{
		return ShaderCreator::CreatePixelShader(DX::g_device, m_pixelShader, path.c_str(), entryPoint.c_str());
	}

	template<>
	inline HRESULT Shader::LoadShader<ID3D11ComputeShader>(const std::wstring path, const std::string entryPoint)
	{
		return ShaderCreator::CreateComputeShader(DX::g_device, m_computeShader, path.c_str(), entryPoint.c_str());
	}

	

}
