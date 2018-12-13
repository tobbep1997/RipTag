#include "EnginePCH.h"
#include "SkyBox.h"


SkyBox::SkyBox() : IRender()
{
	m_skyboxBuffer = nullptr;
	m_rasterizerState = nullptr;
}

SkyBox::~SkyBox()
{
}

HRESULT SkyBox::Init(ForwardRender* forwardRender)
{
	if (!forwardRender)
		return E_INVALIDARG;
	HRESULT hr = 0;
	this->p_forwardRender = forwardRender;
	   
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("SKYBOX_BUFFER", m_skyboxBuffer, sizeof(SkyboxBuffer))))
	{		
		if (SUCCEEDED(hr = DXRHC::CreateRasterizerState("SKYBOX_RASTERIZERSTATE", m_rasterizerState)))
		{
			D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			if (this->m_vertexShader = DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/Shaders/Skybox/SkyboxVertex.hlsl", "main", inputDesc, 4))
			{
				m_inputLayout = DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/Skybox/SkyboxVertex.hlsl");
				if (this->m_pixelShader = DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/Skybox/SkyboxPixel.hlsl"))
				{
					return S_OK;
				}
				throw std::exception("Pixelshader failed");
			}
			throw std::exception("Vertexshader failed");
		}		
	}
	return hr;
}

HRESULT SkyBox::Release()
{
	DX::SafeRelease(m_skyboxBuffer);
	DX::SafeRelease(m_rasterizerState);
	return S_OK;
}

HRESULT SkyBox::Draw()
{
	return E_INVALIDARG;
}

HRESULT SkyBox::Draw(Drawable* drawable, const Camera & camera)
{
	if (!drawable || !drawable->getTexture())
		return E_INVALIDARG;
	   
	StaticMesh * skyBox = drawable->getStaticMesh();
	ID3D11Buffer * vertexBuffer = drawable->getStaticMesh()->getBuffer();
	if (!vertexBuffer || !skyBox)
	{
		return E_INVALIDARG;
	}
	HRESULT hr;
	UINT offset = 0;
	UINT stride = sizeof(StaticVertex);

	_createViewProjection(camera);

	if (FAILED(hr = DXRHC::MapBuffer(m_skyboxBuffer, &m_skyboxValues, sizeof(m_skyboxValues), 6, 1, ShaderTypes::vertex)))
		return hr;
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/Skybox/SkyboxVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/Skybox/SkyboxVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/Skybox/SkyboxPixel.hlsl"), nullptr, 0);
	DX::g_deviceContext->RSSetState(m_rasterizerState);


	drawable->getTexture()->Bind(1);

	DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DX::g_deviceContext->Draw(skyBox->getVertice().size(), 0);
	return S_OK;
}

HRESULT SkyBox::Draw(std::vector<DX::INSTANCING::GROUP> group, const Camera & camera)
{
	return E_FAIL;
}

void SkyBox::_createViewProjection(const Camera & camera)
{
	using namespace DirectX;
	const XMVECTOR zeropos = { 0,0,0,1 };
	
	const XMMATRIX view = XMMatrixLookToLH(zeropos,
		XMLoadFloat4A(&camera.getDirection()), 
		XMLoadFloat4A(&camera.getUP()));

	const XMMATRIX viewProjection = XMLoadFloat4x4A(&camera.getProjection()) * XMMatrixTranspose(view);

	XMStoreFloat4x4A(&m_skyboxValues.ViewProjection, viewProjection);
}
