#include "Engine2DPCH.h"
#include "Render2D.h"
#include "RipTagExtern/RipExtern.h"
#include <d3d11_4.h>


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

	HRESULT hr;
	if (SUCCEEDED(hr = DXRHC::CreateSamplerState("2D Sampler", m_sampler, D3D11_TEXTURE_ADDRESS_WRAP))) { }
	if (SUCCEEDED(hr = DXRHC::CreateBlendState("Render2D Blendstate" ,m_blendState))) { }
	m_spriteBatch = new DirectX::SpriteBatch(DX::g_deviceContext);

	D3D11_DEPTH_STENCIL_DESC dpd{};
	dpd.DepthEnable = TRUE;
	dpd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dpd.DepthFunc = D3D11_COMPARISON_LESS;

	//Create the Depth/Stencil View
	
	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilState(&dpd, &m_depthStencilState)))
	{
		DX::SetName(m_depthStencilState, "Render2D m_depthStencilState");
	}
	if (SUCCEEDED(hr = DXRHC::CreateConstantBuffer("2D HUDTypeStruct" ,m_HUDTypeBuffer, sizeof(HUDTypeStruct)))) { }
	

#ifndef _DEPLOY
	DBG_INIT();
#endif

}

void Render2D::GUIPass()
{
	HRESULT hr;
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

#ifndef _DEPLOY
	//DBG();
#endif
	

	UINT32 vertexSize = sizeof(Quad::QUAD_VERTEX);
	UINT32 offset = 0;

	for (unsigned int j = 0; j < DX::g_2DQueue.size(); j++)
	{
		ID3D11Buffer * vertexBuffer = DX::g_2DQueue[j]->getVertexBuffer();
		Quad * q = DX::g_2DQueue[j];
		HUDTypeEnum type = (HUDTypeEnum)q->getType();
		m_HUDTypeValues.type.x = (unsigned int)type;
		m_HUDTypeValues.color = q->getColor();
		m_HUDTypeValues.outlineColor = q->getOutlineColor();
		switch (type)
		{
		case Render2D::QuadType:
			m_HUDTypeValues.center.x = q->getU();
			m_HUDTypeValues.center.y = q->getV();
			break;
		case Render2D::CircleType:
			m_HUDTypeValues.center.x = q->getAngle();
			m_HUDTypeValues.center.y = q->getCenter().y;
			m_HUDTypeValues.center.z = q->getRadie();
			m_HUDTypeValues.center.w = q->getInnerRadie();
			break;
		case Render2D::OutlinedQuad:
		{
				auto scl = q->getScale();
				float dx = 1 / (scl.x * 0.5f * InputHandler::getViewportSize().x);
				float dy = 1 / (scl.y * 0.5f * InputHandler::getViewportSize().y);
				m_HUDTypeValues.center.x = q->getU();
				m_HUDTypeValues.center.y = q->getV();
				m_HUDTypeValues.center.z = dx * q->getRadie();
				m_HUDTypeValues.center.w = dy * q->getRadie();
		}
			break;
		default:
			break;
		}
		
		if (vertexBuffer != NULL)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			void* dataPtr;
			DX::g_deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			dataPtr = (void*)mappedResource.pData;
			memcpy(dataPtr, q->quadVertex, q->GetSize());
			DX::g_deviceContext->Unmap(vertexBuffer, 0);

			if (SUCCEEDED(hr = DXRHC::MapBuffer(m_HUDTypeBuffer, &m_HUDTypeValues, sizeof(HUDTypeStruct), 0U, 1U, ShaderTypes::pixel))) { }
			DX::g_2DQueue[j]->MapTexture();

			DX::g_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
			DX::g_deviceContext->Draw(4, 0);

		}
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

		auto posAndSize = DX::g_2DQueue[j]->getReferencePosAndSize();

		DirectX::XMFLOAT2 referencePos = { posAndSize.x, posAndSize.y };
		DirectX::XMFLOAT2 size = { posAndSize.z * 0.25f, posAndSize.w * 0.25f};

		referencePos.x = 0.5f * referencePos.x + 0.5f;
		referencePos.y = -0.5f * referencePos.y + 0.5f;

		DirectX::XMVECTOR origin;
		DirectX::XMFLOAT2 pos;
		DirectX::XMVECTOR vpos;
		switch (DX::g_2DQueue[j]->getTextAlignment())
		{
		case Quad::TextAlignment::centerAligned:

			origin = DX::g_2DQueue[j]->getSpriteFont().MeasureString(wstring.data());
			pos = DirectX::XMFLOAT2A(
				(referencePos.x + size.x)  * InputHandler::getViewportSize().x,
				((referencePos.y - size.y)) * InputHandler::getViewportSize().y
			);
			break;
		case Quad::TextAlignment::leftAligned:

			origin = DX::g_2DQueue[j]->getSpriteFont().MeasureString(wstring.data());
			origin = DirectX::XMVectorSetX(origin, 0.0f);
			pos = DirectX::XMFLOAT2A(
				referencePos.x * InputHandler::getViewportSize().x,
				((referencePos.y - size.y)) * InputHandler::getViewportSize().y
			);
			break;
		case Quad::TextAlignment::rightAligned:

			origin = DX::g_2DQueue[j]->getSpriteFont().MeasureString(wstring.data());
			origin = DirectX::XMVectorSetX(origin, DirectX::XMVectorGetX(origin) * 2.0f);
			pos = DirectX::XMFLOAT2A(
				(referencePos.x + size.x * 2.0f) * InputHandler::getViewportSize().x,
				((referencePos.y - size.y)) * InputHandler::getViewportSize().y
			);
			break;
		}

		origin = DirectX::XMVectorScale(origin, 0.5f);
		vpos = DirectX::XMLoadFloat2(&pos);
		DirectX::XMVECTOR color = DirectX::XMLoadFloat4A(&DX::g_2DQueue[j]->getTextColor());

		//Have the scaling based on the Resolution - reference value is 1280p, adjustment multiplier is 0.75f
		float fontScaling = (DX::g_backBufferResolution.right / 1280.f) * 0.75f;
		
		DX::g_2DQueue[j]->getSpriteFont().DrawString(
			m_spriteBatch,
			wstring.data(),
			vpos,
			color,
			0.0f,
			origin,
			fontScaling
		);

		m_spriteBatch->End();
	}

	DX::g_2DQueue.clear();
	DX::g_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#ifndef _DEPLOY
	if (dbg_quad)
	{
		dbg_quad->Release();
		delete dbg_quad;
		dbg_quad = nullptr;
	}
#endif

}

void Render2D::Release()
{
	DX::SafeRelease(m_sampler);
	DX::SafeRelease(m_blendState);
	DX::SafeRelease(m_depthStencilState);
	DX::SafeRelease(m_HUDTypeBuffer);
	delete m_spriteBatch;

#ifndef _DEPLOY
	DBG_RELEASE();
#endif
}

#ifndef _DEPLOY
void Render2D::DBG_INIT()
{
	m_currentProcessID = GetCurrentProcessId();

	ret_code = ::CreateDXGIFactory(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgifactory));
	Manager::g_textureManager.loadTextures("DAB");

}

void Render2D::DBG()
{
	if (SUCCEEDED(ret_code))
	{
		if (SUCCEEDED(dxgifactory->EnumAdapters(0, &dxgiAdapter)))
		{

			if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&dxgiAdapter4)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					memoryUsageVRam = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB
				};


			}
			dxgiAdapter->Release();
			dxgiAdapter4->Release();
		}

	}

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_currentProcessID);


	if (NULL == hProcess)
		return;
	PROCESS_MEMORY_COUNTERS pmc{};
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		memoryUsageRam = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MiB
	}

	CloseHandle(hProcess);

	m_statex.dwLength = sizeof(m_statex);

	GlobalMemoryStatusEx(&m_statex);


	dbg_quad = new Quad();
	dbg_quad->init();
	dbg_quad->setPivotPoint(Quad::PivotPoint::upperRight);
	dbg_quad->setTextAlignment(Quad::TextAlignment::leftAligned);
	dbg_quad->setFont(FontHandler::getFont("consolas16"));
	dbg_quad->setString("VRAM: " + std::to_string((UINT)memoryUsageVRam) + "\nRAM:  " + std::to_string((UINT)(memoryUsageRam - memoryUsageVRam)));

	if (memoryUsageVRam < 256.0f && memoryUsageRam - memoryUsageVRam < 256.0f)
		dbg_quad->setTextColor(DirectX::XMFLOAT4A(0, 1, 0, 1));
	else
		dbg_quad->setTextColor(DirectX::XMFLOAT4A(1, 0, 0, 1));

	dbg_quad->setPosition(1, 1);
	dbg_quad->setScale(.20f, .2f);
	dbg_quad->setColor(1, 1, 1,1);
	dbg_quad->setUnpressedTexture("DAB");
	dbg_quad->setPressedTexture("");
	dbg_quad->setHoverTexture("");
	dbg_quad->Draw();
}

void Render2D::DBG_RELEASE()
{
	dxgifactory->Release();

}
#endif

