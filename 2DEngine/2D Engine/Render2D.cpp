#include "Engine2DPCH.h"
#include "Render2D.h"



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
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	DX::g_shaderManager.VertexInputLayout(L"../Engine/EngineSource/Shader/Shaders/2DVertex.hlsl", "main", inputDesc, 2);
	DX::g_shaderManager.LoadShader<ID3D11PixelShader>(L"../Engine/EngineSource/Shader/Shaders/2DPixel.hlsl");

	HRESULT hr = DXRHC::CreateSamplerState(m_sampler, D3D11_TEXTURE_ADDRESS_WRAP);
	DXRHC::CreateBlendState(m_blendState);
	m_spriteBatch = new DirectX::SpriteBatch(DX::g_deviceContext);

	D3D11_DEPTH_STENCIL_DESC dpd{};
	dpd.DepthEnable = TRUE;
	dpd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dpd.DepthFunc = D3D11_COMPARISON_LESS;

	//Create the Depth/Stencil View
	DX::g_device->CreateDepthStencilState(&dpd, &m_depthStencilState);

	DXRHC::CreateConstantBuffer(m_HUDTypeBuffer, sizeof(HUDTypeStruct));

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
	DX::g_deviceContext->PSSetSamplers(4, 1, &m_sampler);
	DX::g_deviceContext->OMSetDepthStencilState(m_depthStencilState, NULL);
	DX::g_deviceContext->OMSetBlendState(m_blendState, 0, 0xffffffff);

	

	UINT32 vertexSize = sizeof(Quad::QUAD_VERTEX);
	UINT32 offset = 0;

	for (unsigned int j = 0; j < DX::g_2DQueue.size(); j++)
	{
		ID3D11Buffer * vertexBuffer = DX::g_2DQueue[j]->getVertexBuffer();
		Quad * q = DX::g_2DQueue[j];
		HUDTypeEnum type = (HUDTypeEnum)q->getType();

		m_HUDTypeValues.center.x = q->getCenter().x;
		m_HUDTypeValues.center.y = q->getCenter().y;
		m_HUDTypeValues.center.z = q->getRadie();
		m_HUDTypeValues.type.x = (unsigned int)type;

		DXRHC::MapBuffer(m_HUDTypeBuffer, &m_HUDTypeValues, sizeof(HUDTypeStruct), 0U, 1U, ShaderTypes::pixel);
		DX::g_2DQueue[j]->MapTexture();

		DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
		DX::g_deviceContext->Draw(4, 0);
	}
	for (unsigned int j = 0; j < DX::g_2DQueue.size(); j++)
	{
		if (DX::g_2DQueue[j]->getString() == "")
			continue;
		//Draw font on quad
		m_spriteBatch->Begin();

		std::wstring wstring = std::wstring(
			DX::g_2DQueue[j]->getString().begin(),
			DX::g_2DQueue[j]->getString().end());

		DirectX::XMVECTOR origin;

		DirectX::XMVECTOR pos;
		switch (DX::g_2DQueue[j]->getTextAlignment())
		{
		case Quad::TextAlignment::centerAligned:

			origin = DX::g_2DQueue[j]->getSpriteFont().MeasureString(wstring.data());
			pos = DirectX::XMLoadFloat2A(
				&DirectX::XMFLOAT2A(DX::g_2DQueue[j]->getPosition().x * InputHandler::getViewportSize().x,
								   (1.0f - DX::g_2DQueue[j]->getPosition().y) * InputHandler::getViewportSize().y));
			break;
		case Quad::TextAlignment::leftAligned:

			origin = DX::g_2DQueue[j]->getSpriteFont().MeasureString(L"");
			pos = DirectX::XMLoadFloat2A(
				&DirectX::XMFLOAT2A((DX::g_2DQueue[j]->getPosition().x - (DX::g_2DQueue[j]->getSize().x / 4.0f)) * InputHandler::getViewportSize().x,
								   (1.0f - DX::g_2DQueue[j]->getPosition().y - (DX::g_2DQueue[j]->getSize().y / 6.0f)) * InputHandler::getViewportSize().y));

			break;
		default:
			break;
		}

		origin = DirectX::XMVectorScale(origin, 0.5f);

		//pos = DirectX::XMLoadFloat2A(
		//	&DirectX::XMFLOAT2A(DX::g_2DQueue[j]->getPosition().x * InputHandler::getViewportSize().x, (1.0f - DX::g_2DQueue[j]->getPosition().y) * InputHandler::getViewportSize().y));

		DirectX::XMVECTOR color = DirectX::XMLoadFloat4A(&DX::g_2DQueue[j]->getTextColor());

		DX::g_2DQueue[j]->getSpriteFont().DrawString(
			m_spriteBatch,
			wstring.data(),
			pos,
			color,
			0.0f,
			origin
		);

		m_spriteBatch->End();
	}

	DX::g_2DQueue.clear();
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Render2D::Release()
{
	DX::SafeRelease(m_sampler);
	DX::SafeRelease(m_blendState);
	DX::SafeRelease(m_depthStencilState);
	DX::SafeRelease(m_HUDTypeBuffer);
	delete m_spriteBatch;
}


