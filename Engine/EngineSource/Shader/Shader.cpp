#include "EnginePCH.h"
#include "Shader.h"


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
	void Shader::setInputLayout(ID3D11InputLayout *& inputLayout)
	{
		this->m_inputLayout = inputLayout;
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
	void Shader::ReleaseWithoutInputLayout()
	{
		DX::SafeRelease(m_vertexShader);
		DX::SafeRelease(m_domainShader);
		DX::SafeRelease(m_hullShader);
		DX::SafeRelease(m_geometryShader);
		DX::SafeRelease(m_pixelShader);
		DX::SafeRelease(m_computeShader);
	}
	HRESULT Shader::ReloadShader()
	{
		HRESULT hr;
		switch (m_type)
		{
		case Shaders::Vertex:
		{
			ID3D11VertexShader * vs = m_vertexShader;
			hr = LoadShader<ID3D11VertexShader>(this->m_path, this->m_entryPoint);
			if (SUCCEEDED(hr))
				DX::SafeRelease(vs);
			break;
		}
		case Shaders::domain:
		{
			ID3D11DomainShader * ds = m_domainShader;
			hr = LoadShader<ID3D11DomainShader>(this->m_path, this->m_entryPoint);
			if (SUCCEEDED(hr))
				DX::SafeRelease(ds);
			break;
		}
		case Shaders::Hull:
		{
			ID3D11HullShader * hs = m_hullShader;
			hr = LoadShader<ID3D11HullShader>(this->m_path, this->m_entryPoint);
			if (SUCCEEDED(hr))
				DX::SafeRelease(hs);
			break;
		}
		case Shaders::Geometry:
		{
			ID3D11GeometryShader * gs = m_geometryShader;
			hr = LoadShader<ID3D11GeometryShader>(this->m_path, this->m_entryPoint);
			if (SUCCEEDED(hr))
				DX::SafeRelease(gs);
			break;
		}
		case Shaders::Pixel:
		{
			ID3D11PixelShader * ps = m_pixelShader;
			hr = LoadShader<ID3D11PixelShader>(this->m_path, this->m_entryPoint);
			if (SUCCEEDED(hr))
				DX::SafeRelease(ps);
			break;
		}
		case Shaders::Compute:
		{
			ID3D11ComputeShader * cs = m_computeShader;
			hr = LoadShader<ID3D11ComputeShader>(this->m_path, this->m_entryPoint);
			if (SUCCEEDED(hr))
				DX::SafeRelease(cs);
			break;
		}
		default:
			break;
		}
		return hr;
	}
}