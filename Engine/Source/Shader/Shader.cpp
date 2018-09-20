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
		m_computeShader(nullptr),
		m_inputLayout(nullptr)
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
	void Shader::setEntryPoint(const std::string & entryPoint)
	{
		this->m_entryPoint = entryPoint;
	}
	std::string Shader::getEntryPoint() const
	{
		return this->m_entryPoint;
	}
	ID3D11VertexShader * Shader::VertexInputLayout(const std::wstring path, const std::string entryPoint, D3D11_INPUT_ELEMENT_DESC inputDesc[], unsigned int size)
	{
		ShaderCreator::CreateVertexShader(DX::g_device, m_vertexShader, path.c_str(), entryPoint.c_str(), inputDesc, size, m_inputLayout);
		return m_vertexShader;
	}
	ID3D11InputLayout * Shader::getInputLayout()
	{
		return m_inputLayout;
	}
	void Shader::Release()
	{
		DX::SafeRelease(m_vertexShader);
		DX::SafeRelease(m_domainShader);
		DX::SafeRelease(m_hullShader);
		DX::SafeRelease(m_geometryShader);
		DX::SafeRelease(m_pixelShader);
		DX::SafeRelease(m_computeShader);
		DX::SafeRelease(m_inputLayout);
	}
}