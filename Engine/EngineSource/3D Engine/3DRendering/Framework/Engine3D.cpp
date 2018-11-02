#include "Engine3D.h"
#include "../../Extern.h"

ID3D11Device*			DX::g_device;
ID3D11DeviceContext1*	DX::g_deviceContext;

Shaders::ShaderManager DX::g_shaderManager;

std::vector<Drawable*> DX::g_geometryQueue;
std::vector<Drawable*> DX::g_animatedGeometryQueue;

std::vector<PointLight*> DX::g_lights;

std::vector<Drawable*> DX::g_visabilityDrawQueue;

std::vector<Drawable*> DX::g_wireFrameDrawQueue;

std::vector<Quad*> DX::g_2DQueue;

std::vector<VisibilityComponent*> DX::g_visibilityComponentQueue;

MeshManager Manager::g_meshManager;
TextureManager Manager::g_textureManager;
 

void DX::SafeRelease(IUnknown * unknown)
{
	if (unknown)
		unknown->Release();
	unknown = nullptr;
}

Engine3D::Engine3D()
{
	
}


Engine3D::~Engine3D()
{
}

HRESULT Engine3D::Init(HWND hwnd, bool fullscreen, UINT width, UINT hight)
{
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //DEBUG

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	D3D_FEATURE_LEVEL fl_in[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL fl_out = D3D_FEATURE_LEVEL_11_0;

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hwnd;								// the window to be used
	scd.SampleDesc.Count = m_sampleCount;                   // how many multisamples
	scd.Windowed = !fullscreen;								// windowed/full-screen mode
	
	

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

		m_swapChain->ResizeBuffers(0, width, hight, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		DX::g_device->CreateRenderTargetView(pBackBuffer, NULL, &m_backBufferRTV);
		//we are creating the standard depth buffer here.
		_createDepthSetencil(width, hight);
		_initViewPort(width, hight);

	
		//DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);	//As a standard we set the rendertarget. But it will be changed in the prepareGeoPass
		pBackBuffer->Release();
	}
	m_forwardRendering.Init(m_swapChain, m_backBufferRTV, m_depthStencilView, m_depthBufferTex, m_samplerState, m_viewport);
	return hr;
}

void Engine3D::Flush(Camera & camera)
{
	
	m_forwardRendering.Flush(camera);
}

void Engine3D::Clear()
{
	this->m_forwardRendering.Clear();
}

void Engine3D::Present()
{
	m_swapChain->Present(1, 0);
}

void Engine3D::Release()
{
	m_swapChain->SetFullscreenState(false, NULL);

	DX::SafeRelease(DX::g_device);
	DX::SafeRelease(DX::g_deviceContext);
	DX::SafeRelease(m_swapChain);
	DX::SafeRelease(m_backBufferRTV);
	DX::SafeRelease(m_depthStencilView);
	DX::SafeRelease(m_depthBufferTex);
	DX::SafeRelease(m_samplerState);	

	m_forwardRendering.Release();
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

	//Create the Depth/Stencil View
	HRESULT hr = DX::g_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depthBufferTex);
	hr = DX::g_device->CreateDepthStencilView(m_depthBufferTex, NULL, &m_depthStencilView);
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
