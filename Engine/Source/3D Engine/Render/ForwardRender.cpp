#include "ForwardRender.h"
#include "../Extern.h"
#include "../../ShaderCreator.h"

ForwardRender::ForwardRender()
{
	m_lastVertexPath = L"NULL";
	m_lastPixelPath = L"NULL";
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
	


	
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	DX::g_shaderManager.VertexInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl", "main", inputDesc, 4);
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/Source/Shader/PixelShader.hlsl");

	_CreateConstantBuffer();
	this->CREATE_VIEWPROJ();
}

struct TriangleVertex
{
	float x, y, z, w;
	float r, g, b, a;
};

void ForwardRender::GeometryPass()
{
	float c[4] = { 1.0f,0.0f,1.0f,1.0f };
	
	DX::g_deviceContext->ClearRenderTargetView(m_backBufferRTV, c);	
	DX::g_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX::g_deviceContext->IASetInputLayout(DX::g_shaderManager.GetInputLayout(L"../Engine/Source/Shader/VertexShader.hlsl"));
	DX::g_deviceContext->RSSetViewports(1, &m_viewport);
	DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, nullptr);
	

	for (unsigned int i = 0; i < DX::g_geometryQueue.size(); i++)
	{
		UINT32 vertexSize = sizeof(StaticVertex);
		UINT32 offset = 0;
		
		_SetShaders(i);

		ID3D11Buffer * vertexBuffer = DX::g_geometryQueue[i]->getBuffer();
		m_values.worldMatrix = DX::g_geometryQueue[i]->getWorldmatrix();
		DX::g_geometryQueue[i]->addRotation(0, 0.001f);

		_mapTempConstantBuffer();
		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(DX::g_geometryQueue[i]->VertexSize(), 0);

	}

	DX::g_geometryQueue.clear();
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
	DX::SafeRelease(m_tempConstant);
}


void ForwardRender::_CreateConstantBuffer()
{
	D3D11_BUFFER_DESC exampleBufferDesc;
	exampleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	exampleBufferDesc.ByteWidth = sizeof(tempCPU);
	exampleBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	exampleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	exampleBufferDesc.MiscFlags = 0;
	exampleBufferDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = 0;
	hr = DX::g_device->CreateBuffer(&exampleBufferDesc, nullptr, &m_tempConstant);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		exit(-1);
	}
}

void ForwardRender::_mapTempConstantBuffer()
{
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	DX::g_deviceContext->Map(m_tempConstant, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);
	// copy memory from CPU to GPU the entire struct
	//m_values.val1 += 0.001;
	m_values.view = view;
	m_values.projection = projection;

	memcpy(dataPtr.pData, &m_values, sizeof(tempCPU));
	// UnMap constant buffer so that we can use it again in the GPU
	DX::g_deviceContext->Unmap(m_tempConstant, 0);
	// set resource to Vertex Shader
	DX::g_deviceContext->VSSetConstantBuffers(0, 1, &m_tempConstant);
}

void ForwardRender::CREATE_VIEWPROJ()
{
	using namespace DirectX;

	XMFLOAT3 cameraStored = XMFLOAT3(0, 0, -5);

	XMFLOAT3 lookAtStored = XMFLOAT3(0, 0, 1);

	XMFLOAT3 UP_STORED = XMFLOAT3(0, 1, 0);



	XMVECTOR cameraPos = XMLoadFloat3(&cameraStored);

	XMVECTOR lookAt = XMLoadFloat3(&lookAtStored);

	XMVECTOR UP = XMLoadFloat3(&UP_STORED);
	XMStoreFloat4x4A(&this->projection, XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI * 0.5f, 1/1, 1, 20)));
	XMStoreFloat4x4A(&this->view,XMMatrixTranspose(XMMatrixLookToLH(cameraPos, lookAt, UP)));

}

void ForwardRender::_SetShaders(int i)
{
	if (m_lastVertexPath != DX::g_geometryQueue[i]->getVertexPath())
	{
		DX::g_deviceContext->VSSetShader(DX::g_shaderManager.LoadShader<ID3D11VertexShader>(DX::g_geometryQueue[i]->getVertexPath()), nullptr, 0);
	}
	DX::g_deviceContext->HSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->DSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->GSSetShader(nullptr, nullptr, 0);
	DX::g_deviceContext->PSSetShader(nullptr, nullptr, 0);
	if (m_lastPixelPath != DX::g_geometryQueue[i]->getPixelPath())
	{
		DX::g_deviceContext->PSSetShader(DX::g_shaderManager.LoadShader<ID3D11PixelShader>(DX::g_geometryQueue[i]->getPixelPath()), nullptr, 0);
	}
}
