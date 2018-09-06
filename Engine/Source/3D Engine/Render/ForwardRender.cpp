#include "ForwardRender.h"
#include "../Extern.h"
#include "../../ShaderCreator.h"

ForwardRender::ForwardRender()
{


}


ForwardRender::~ForwardRender()
{
}

void ForwardRender::Init(	IDXGISwapChain*				swapChain,
							ID3D11RenderTargetView*		backBufferRTV,
							ID3D11DepthStencilView*		depthStencilView,
							ID3D11Texture2D*			depthBufferTex,
							ID3D11SamplerState*			samplerState,
							D3D11_VIEWPORT				viewport)
{
	m_swapChain = swapChain;
	m_backBufferRTV = backBufferRTV;
	m_depthStencilView = depthStencilView;
	m_depthBufferTex = depthBufferTex;
	m_samplerState = samplerState;
	m_viewport = viewport;
	
	//RIP
	//m_vertexShader = m_sm.LoadShader<ID3D11VertexShader>(L"../Engine/Source/Shader/VertexShader.hlsl");
	//HRESULT hr = DX::g_device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), blob->GetBufferPointer(), blob->GetBufferSize(), &m_inputLayout);

	ID3DBlob * blob = nullptr;
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	//ShaderCreator::CreateVertexShader(DX::g_device, m_vertexShader, L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 2, m_inputLayout);
	//ShaderCreator::CreatePixelShader(DX::g_device, m_pixelShader, L"../Engine/Source/Shader/PixelShader.hlsl");
	m_vertexShader = DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 2);
	m_pixelShader =  DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");
}

struct TriangleVertex
{
	float x, y, z, w;
	float r, g, b, a;
};

void ForwardRender::GeometryPass()
{
	float c[4] = { 1.0f,0.0f,1.0f,1.0f };

	//DX::g_deviceContext->ClearState();
	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);	
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//DX::g_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//DX::g_deviceContext->OMSetBlendState(nullptr, 0, 0xffffffff);


	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	


	TriangleVertex triangleVertices[3] =
	{
		0.0f, 0.5f, 0.0f, 1.0f,  
		0.0f, 0.0f,  1.0f, 1.0f,  

		0.5f, -0.5f, 0.0f, 1.0f,
		1.0f, 1.0f,  1.0f, 1.0f, 
		
		-0.5f, -0.5f, 0.0f, 1.0f,
		1.0f, 1.0f,  1.0f, 1.0f  

	};

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;

	ID3D11Buffer * vertexBuffer;

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(TriangleVertex) * 3;


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = triangleVertices;
	HRESULT hr = DX::g_device->CreateBuffer(&bufferDesc, &vertexData, &vertexBuffer);
	DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);

	DX::g_deviceContext->Draw(3, 0);

	vertexBuffer->Release();
}

void ForwardRender::Flush()
{
	m_swapChain->Present(0, 0);
}

void ForwardRender::Present()
{
}

void ForwardRender::Release()
{


}

