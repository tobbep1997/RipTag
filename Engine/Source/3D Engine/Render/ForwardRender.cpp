#include "ForwardRender.h"
#include "../Extern.h"


ForwardRender::ForwardRender()
{

	ID3DBlob * blob = nullptr;
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	HRESULT hr = DX::g_device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), blob->GetBufferPointer(), blob->GetBufferSize(), &m_inputLayout);


}


ForwardRender::~ForwardRender()
{
}

void ForwardRender::GeometryPass()
{
/*
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(m_inputLayout);
	DX::g_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);
	*/
}
