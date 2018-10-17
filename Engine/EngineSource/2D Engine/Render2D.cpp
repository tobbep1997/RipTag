#include "Render2D.h"
#include "../3D Engine/Extern.h"
#include "../Shader/ShaderManager.h"

Render2D::Render2D()
{
}


Render2D::~Render2D()
{
}

void Render2D::Init()
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/Shaders/2DVertex.hlsl", "main", inputDesc, 2);
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/2DPixel.hlsl");

	DXRHC::CreateSamplerState(m_sampler);
	DXRHC::CreateConstantBuffer(m_positionBuffer, sizeof(Transform2D::Transform2DBuffer));
}

void Render2D::GUIPass()
{
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/EngineSource/Shader/Shaders/2DVertex.hlsl"));
	DX::g_deviceContext->VSSetShader(DX::g_shaderManager.GetShader<ID3D11VertexShader>(L"../Engine/EngineSource/Shader/Shaders/2DVertex.hlsl"), nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(DX::g_shaderManager.GetShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/2DPixel.hlsl"), nullptr, 0);
	DX::g_deviceContext->PSSetSamplers(0, 1, &m_sampler);
	

	for (unsigned int j = 0; j < DX::g_2DQueue.size(); j++)
	{
		UINT32 vertexSize = sizeof(Quad::QUAD_VERTEX);
		UINT32 offset = 0;

		ID3D11Buffer * vertexBuffer = DX::g_2DQueue[j]->getVertexBuffer();
		_mapBuffers(DX::g_2DQueue[j]);
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_geometryQueue[j]->getVertexSize(), 0);
	}
	DX::g_2DQueue.clear();
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Render2D::Release()
{
	DX::SafeRelease(m_sampler);
	DX::SafeRelease(m_positionBuffer);
}

void Render2D::_mapBuffers(Quad * quad)
{
	m_positionValues.m_position = quad->getPosition();
	m_positionValues.m_size = quad->getSize();

	DXRHC::MapBuffer(m_positionBuffer, &m_positionValues, sizeof(Transform2D::Transform2DBuffer), 3, 1, ShaderTypes::vertex);
}
