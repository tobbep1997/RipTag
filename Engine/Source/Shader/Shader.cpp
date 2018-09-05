#include "Shader.h"

#include "../ShaderCreator.h"
#include "../3D Engine/Extern.h"


namespace Shaders
{

	Shader::Shader() :
		m_vertexShader(nullptr),
		m_domainShader(nullptr),
		m_hullShader(nullptr),
		m_geometryShader(nullptr),
		m_pixelShader(nullptr),
		m_computeShader(nullptr)
	{
	}


	Shader::~Shader()
	{
	}


	void Shader::setPath(const std::wstring & path)
	{
		this->m_path = path;
	}
	void Shader::setKey(const unsigned int key)
	{
		this->m_key = key;
	}
	void Shader::setType(const ShaderType & shaderType)
	{
		this->m_type = shaderType;
	}
	std::wstring Shader::getPath() const
	{
		return this->m_path;
	}
	unsigned int Shader::getKey() const
	{
		return this->m_key;
	}
	ShaderType Shader::getType() const
	{
		return this->m_type;
	}
	/*
	void Shader::LoadShader(ShaderType type, const std::wstring path, const std::string entryPoint)
	{
		switch (type)
		{
		case Shaders::Vertex:
			ShaderCreator::CreateVertexShader(DX::g_device, m_vertexShader, path.c_str(), entryPoint.c_str());
			break;
		case Shaders::domain:
			break;
		case Shaders::Hull:
			break;
		case Shaders::Geometry:
			break;
		case Shaders::Pixel:
			ShaderCreator::CreatePixelShader(DX::g_device, m_pixelShader, path.c_str(), entryPoint.c_str());
			break;
		case Shaders::Compute:
			break;
		}
		setPath(path);
		setType(type);
	}*/

	void Shader::Release()
	{
		DX::SafeRelease(m_vertexShader);
		DX::SafeRelease(m_domainShader);
		DX::SafeRelease(m_hullShader);
		DX::SafeRelease(m_geometryShader);
		DX::SafeRelease(m_pixelShader);
		DX::SafeRelease(m_computeShader);
	}
}