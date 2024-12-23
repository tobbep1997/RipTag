#include "EnginePCH.h"
#include "Engine3D.h"

ID3D11Device*								DX::g_device;
ID3D11DeviceContext1*						DX::g_deviceContext;
Shaders::ShaderManager						DX::g_shaderManager;
Drawable*									DX::g_player;
Drawable*									DX::g_remotePlayer = nullptr;
std::vector<Drawable*>						DX::g_animatedGeometryQueue;
std::vector<PointLight*>					DX::g_lights;
std::vector<PointLight*>					DX::g_prevlights;
std::vector<Drawable*>						DX::g_outlineQueue;
std::vector<Drawable*>						DX::g_wireFrameDrawQueue;
std::vector<Quad*>							DX::g_2DQueue;
std::vector<VisibilityComponent*>			DX::g_visibilityComponentQueue;
std::vector<ParticleEmitter*>				DX::g_emitters;
RECT										DX::g_backBufferResolution;

bool DX::g_screenShootCamera = false;

MeshManager Manager::g_meshManager;
TextureManager Manager::g_textureManager;

std::vector<DX::INSTANCING::GROUP> DX::INSTANCING::g_instanceGroups;
std::vector<DX::INSTANCING::GROUP> DX::INSTANCING::g_instanceWireFrameGroups;

std::vector<DX::INSTANCING::GROUP> DX::INSTANCING::g_instanceShadowGroups;

bool Cheet::g_visabilityDisabled = false;
bool Cheet::g_DBG_CAM = false;

Drawable *	DX::g_skyBox = nullptr;

bool checkLoltest5(Drawable* drawable, PointLight * pl)
{
	using namespace DirectX;
	BoundingBox bb = BoundingBox(XMFLOAT3(pl->getPosition().x, pl->getPosition().y, pl->getPosition().z),
								 XMFLOAT3(pl->getFarPlane(), pl->getFarPlane(), pl->getFarPlane()));
	if (drawable->getBoundingBox())
		return drawable->getBoundingBox()->Intersects(bb);
	return false;
}

void DX::INSTANCING::submitToShadowQueueInstance(Drawable* drawable)
{
	using namespace DX::INSTANCING;
	std::vector<GROUP> * queue = nullptr;
	if (drawable->getCastShadows())
		queue = &g_instanceShadowGroups;

	auto lol = false;
	for (int i = 0; i < DX::g_lights.size() && !lol; i++)
	{
		if (checkLoltest5(drawable, DX::g_lights[i]))
			lol = true;
	}
	if (!queue || !lol)
		return;

	auto exisitingEntry = std::find_if(queue->begin(), queue->end(), [&](const GROUP& item) {
		return drawable->getStaticMesh() == item.staticMesh && drawable->getTextureName() == item.textureName;
	});

	OBJECT attribute;

	attribute.worldMatrix = drawable->getWorldmatrix();
	attribute.objectColor = drawable->getColor();
	attribute.textureTileMult = DirectX::XMFLOAT4A(drawable->getTextureTileMult().x, drawable->getTextureTileMult().y, 0, 0);
	attribute.usingTexture.x = drawable->isTextureAssigned();


	if (exisitingEntry == queue->end())
	{
		GROUP newGroup;

		newGroup.attribs.push_back(attribute);
		newGroup.staticMesh = drawable->getStaticMesh();
		newGroup.textureName = drawable->getTextureName();
		queue->push_back(newGroup);
	}
	else
	{
		exisitingEntry->attribs.push_back(attribute);
	}
}
void DX::INSTANCING::submitToWireframeInstance(Drawable* drawable)
{
	using namespace DX::INSTANCING;
	std::vector<GROUP> * queue = &g_instanceWireFrameGroups;

	if (!queue)
		return;

	auto exisitingEntry = std::find_if(queue->begin(), queue->end(), [&](const GROUP& item) {
		return drawable->getStaticMesh() == item.staticMesh;
	});

	OBJECT attribute;

	attribute.worldMatrix = drawable->getWorldmatrix();
	attribute.objectColor = drawable->getColor();
	attribute.textureTileMult = DirectX::XMFLOAT4A(drawable->getTextureTileMult().x, drawable->getTextureTileMult().y, 0, 0);
	attribute.usingTexture.x = drawable->isTextureAssigned();


	if (exisitingEntry == queue->end())
	{
		GROUP newGroup;
		newGroup.attribs.push_back(attribute);
		newGroup.staticMesh = drawable->getStaticMesh();
		newGroup.textureName = "";
		queue->push_back(newGroup);
	}
	else
	{
		exisitingEntry->attribs.push_back(attribute);
	}
}

void DX::INSTANCING::submitToInstance(Drawable* drawable, Camera * camera)
{
	using namespace DirectX;
	using namespace DX::INSTANCING;
	/*
	 * Copyright chefen (c) 2018
	 */
	submitToShadowQueueInstance(drawable);
	std::vector<GROUP> * queue = nullptr;
	if (drawable->getEntityType() != EntityType::PlayerType && !drawable->getOutline())
	{
		queue = &g_instanceGroups;
	}
	else if (drawable->getOutline())
	{
		DX::g_outlineQueue.push_back(drawable);
		queue = &g_instanceGroups;
	}
	else if (drawable->getEntityType() == EntityType::PlayerType)
	{
		g_player = drawable;
	}
	else if (drawable->getEntityType() == EntityType::RemotePlayerType)
	{
		g_remotePlayer = drawable;
	}

	if (!queue)
		return;
	XMMATRIX proj, viewInv;
	BoundingFrustum boundingFrustum;
	proj = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&camera->getProjection()));
	viewInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4A(&camera->getView())));
	DirectX::BoundingFrustum::CreateFromMatrix(boundingFrustum, proj);
	boundingFrustum.Transform(boundingFrustum, viewInv);
	if (drawable->getBoundingBox())
		if (!drawable->getBoundingBox()->Intersects(boundingFrustum))
			return;
	auto exisitingEntry = std::find_if(queue->begin(), queue->end(), [&](const GROUP& item) {
		return drawable->getStaticMesh() == item.staticMesh && drawable->getTextureName() == item.textureName;
	});

	OBJECT attribute;
	
	attribute.worldMatrix = drawable->getWorldmatrix();
	attribute.objectColor = drawable->getColor();
	attribute.textureTileMult = DirectX::XMFLOAT4A(drawable->getTextureTileMult().x, drawable->getTextureTileMult().y,0,0);
	attribute.usingTexture.x = drawable->isTextureAssigned();
	

	if (exisitingEntry == queue->end())
	{
		GROUP newGroup;

		newGroup.attribs.push_back(attribute);
		newGroup.staticMesh = drawable->getStaticMesh();
		newGroup.textureName = drawable->getTextureName();
		queue->push_back(newGroup);
	}
	else
	{
		exisitingEntry->attribs.push_back(attribute);
	}

}

std::vector<Drawable*> DX::g_cullQueue;
std::vector<DX::INSTANCING::GROUP> DX::INSTANCING::g_temp;

void DX::INSTANCING::tempInstance(Drawable* drawable)
{
	using namespace DirectX;
	using namespace DX::INSTANCING;
	/*
	 * Copyright chefen (c) 2018
	 */
	submitToShadowQueueInstance(drawable);
	std::vector<GROUP> * queue = &g_temp;



	
	auto exisitingEntry = std::find_if(queue->begin(), queue->end(), [&](const GROUP& item) {
		return drawable->getStaticMesh() == item.staticMesh && drawable->getTextureName() == item.textureName;
	});

	OBJECT attribute;

	attribute.worldMatrix = drawable->getWorldmatrix();
	attribute.objectColor = drawable->getColor();
	attribute.textureTileMult = DirectX::XMFLOAT4A(drawable->getTextureTileMult().x, drawable->getTextureTileMult().y, 0, 0);
	attribute.usingTexture.x = drawable->isTextureAssigned();
	if (drawable->getTexture()->getIndex() != -1)
	{
		attribute.usingTexture.y = 1;
		attribute.usingTexture.z = drawable->getTexture()->getIndex();
	}
	else
	{
		attribute.usingTexture.y = -1;
		attribute.usingTexture.z = drawable->getTexture()->getIndex();
	}


	if (exisitingEntry == queue->end())
	{
		GROUP newGroup;

		newGroup.attribs.push_back(attribute);
		newGroup.staticMesh = drawable->getStaticMesh();
		newGroup.textureName = drawable->getTextureName();
		queue->push_back(newGroup);
	}
	else
	{
		exisitingEntry->attribs.push_back(attribute);
	}

}

void DX::SafeRelease(IUnknown * unknown)
{
	if (unknown)
		unknown->Release();
	unknown = nullptr;
}

void DX::SetName(ID3D11DeviceChild * dc, const std::string & name)
{
#ifndef _DEPLOY
	dc->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(char) * name.size(), name.c_str());
#endif
}

void DX::SetName(ID3D11DeviceChild * dc, const std::wstring & name)
{
#ifndef _DEPLOY
	dc->SetPrivateData(WKPDID_D3DDebugObjectNameW, sizeof(wchar_t) * name.size(), name.c_str());
#endif
}

WindowContext * SettingLoader::g_windowContext;

Engine3D::Engine3D()
{
	
}


Engine3D::~Engine3D()
{
}

HRESULT Engine3D::Init(HWND hwnd, const WindowContext & windContext)
{
	UINT createDeviceFlags = 0;

#ifndef _DEPLOY
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //DEBUG

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	D3D_FEATURE_LEVEL fl_in[] = { D3D_FEATURE_LEVEL_11_1 };
	D3D_FEATURE_LEVEL fl_out = D3D_FEATURE_LEVEL_11_0;

	// fill the swap chain description struct
	scd.BufferCount = 4;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hwnd;								// the window to be used
	scd.SampleDesc.Count = m_sampleCount;                   // how many multisamples
	scd.Windowed = !windContext.fullscreen;								// windowed/full-screen mode
	
	

	ID3D11DeviceContext* pDevResult = nullptr;// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,//D3D11_CREATE_DEVICE_DEBUG,
		fl_in,
		ARRAYSIZE(fl_in),
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&DX::g_device,
		&fl_out,
		&pDevResult);

	if (SUCCEEDED(hr))
	{
		if (SUCCEEDED(pDevResult->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&DX::g_deviceContext)))
		{
			DX::g_deviceContext->Release();
		}

		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		//m_swapChain->ResizeBuffers(0, width, hight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY);
		
		// use the back buffer address to create the render target
		DX::g_deviceContext->OMSetRenderTargets(NULL, NULL, NULL);
		DX::g_deviceContext->Flush();

		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		pBackBuffer->Release();

		m_swapChain->ResizeBuffers(0, windContext.clientWidth, windContext.clientHeight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (SUCCEEDED(hr = DX::g_device->CreateRenderTargetView(pBackBuffer, NULL, &m_backBufferRTV)))
		{
			DX::SetName(m_backBufferRTV, "m_backBufferRTV");
			//we are creating the standard depth buffer here.
			_createDepthSetencil(windContext.clientWidth, windContext.clientHeight);
			_initViewPort(windContext.clientWidth, windContext.clientHeight);

		
			//DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);	//As a standard we set the rendertarget. But it will be changed in the prepareGeoPass
			pBackBuffer->Release();
		}
	}
	m_forwardRendering = new ForwardRender();
	m_forwardRendering->Init(m_swapChain, 
		m_backBufferRTV, 
		m_depthStencilView,
		m_depthStencilState,
		m_depthBufferTex,
		m_samplerState,
		m_viewport,
		windContext);
	return hr;
}

void Engine3D::Flush(Camera & camera)
{	
	
	m_forwardRendering->Flush(camera);
}

void Engine3D::Clear()
{
	this->m_forwardRendering->Clear();
}

void Engine3D::Present()
{
	m_swapChain->Present(0, 0);
}

void Engine3D::Release()
{
	m_swapChain->SetFullscreenState(false, NULL);

	DX::SafeRelease(m_swapChain);
	DX::SafeRelease(m_backBufferRTV);
	DX::SafeRelease(m_depthStencilView);
	DX::SafeRelease(m_depthBufferTex);
	DX::SafeRelease(m_samplerState);	
	DX::SafeRelease(m_depthStencilState);
	
	m_forwardRendering->Release();
	delete m_forwardRendering;

	DX::SafeRelease(DX::g_deviceContext);
	   
#ifndef _DEPLOY
	HRESULT hr;
	if (DX::g_device->Release() > 0)
	{		
		ID3D11Debug* dbg_device = nullptr;
		if (SUCCEEDED(hr = DX::g_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&dbg_device)))
		{
			if (SUCCEEDED(hr = dbg_device->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL)))
			{

			}
			dbg_device->Release();
		}
	}
#endif

}

void Engine3D::_createDepthSetencil(UINT width, UINT hight)
{
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = hight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = m_sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_DESC dpd{};
	dpd.DepthEnable = TRUE;
	dpd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dpd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dpd.StencilEnable = TRUE;

	dpd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dpd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dpd.FrontFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;
	dpd.BackFace.StencilFunc = D3D11_COMPARISON_LESS_EQUAL;
	
	dpd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dpd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	
	dpd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dpd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	dpd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dpd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	//Create the Depth/Stencil View

	HRESULT hr;

	if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilState(&dpd, &m_depthStencilState)))
	{
		DX::SetName(m_depthStencilState, "ENGINE: m_depthStencilState");
		if (SUCCEEDED(hr = DX::g_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthBufferTex)))
		{
			DX::SetName(m_depthBufferTex, "ENGINE: m_depthBufferTex");
			if (SUCCEEDED(hr = DX::g_device->CreateDepthStencilView(m_depthBufferTex, NULL, &m_depthStencilView)))
			{
				DX::SetName(m_depthStencilView, "ENGINE: m_depthStencilView");
			}
		}
	}
	
}

void Engine3D::_initViewPort(UINT width, UINT hight)
{
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(hight);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
}
