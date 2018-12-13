#include "EnginePCH.h"
#include "SkyBox.h"
#include "Source/Game/Actors/Player.h"


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
		if (SUCCEEDED(hr = DXRHC::CreateRasterizerState("SKYBOX_RASTERIZERSTATE", m_rasterizerState, 0, D3D11_CULL_FRONT, 10)))
		{
			D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			if (DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/Skybox/SkyboxVertex.hlsl", "main", inputDesc, 2))
			{
				m_inputLayout = DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/Skybox/SkyboxVertex.hlsl");
				if (DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/Skybox/SkyboxPixel.hlsl"))
				{
					return S_OK;
				}
				throw std::exception("Pixelshader failed");
			}
			throw std::exception("Vertexshader failed");
		}		
		return hr;
	}
	return hr;
}

HRESULT SkyBox::Release()
{
	DX::SafeRelease(m_skyboxBuffer);
	DX::SafeRelease(m_rasterizerState);
}

HRESULT SkyBox::Draw(Drawable* drawable, const Camera & camera)
{
	if (!drawable || !drawable->getTexture())
		return E_INVALIDARG;

	ID3D11Buffer * vertexBuffer;
	if (!(vertexBuffer = drawable->getStaticMesh()->getBuffer()))
	{
		return E_INVALIDARG;
	}
	HRESULT hr;
	UINT offset = 0;
	UINT stride = sizeof(StaticVertex);

	_createViewProjection(camera);

	if (FAILED(hr = DXRHC::MapBuffer(m_skyboxBuffer, &m_skyboxValues, sizeof(m_skyboxValues), 6, 1, ShaderTypes::vertex)))
		return hr;

	DX::g_deviceContext->IASetInputLayout(m_inputLayout);
	DX::g_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	DX::g_deviceContext->RSSetState(m_rasterizerState);

	DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DX::g_deviceContext->Draw(drawable->getStaticMesh()->getSize(), 0);
	return S_OK;
}

HRESULT SkyBox::Draw(std::vector<DX::INSTANCING::GROUP> group, const Camera & camera)
{
	return E_FAIL;
}

void SkyBox::_createViewProjection(const Camera & camera)
{
	using namespace DirectX;

	const XMMATRIX viewProjection = XMMatrixLookToLH(XMVectorZero(), 
		XMLoadFloat4A(&camera.getDirection()), 
		XMLoadFloat4A(&camera.getUP()));

	XMStoreFloat4x4A(&m_skyboxValues.ViewProjection, viewProjection);
}
