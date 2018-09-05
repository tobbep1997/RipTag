#include "Engine3D.h"
#include "Extern.h"

ID3D11Device*			DX::g_device;
ID3D11DeviceContext*	DX::g_deviceContext;

void DX::SafeRelease(IUnknown * unknown)
{
	if (unknown)
		unknown->Release();
	unknown = NULL;
}

Engine3D::Engine3D()
{
}


Engine3D::~Engine3D()
{
}

HRESULT Engine3D::Init(HWND hwnd, bool fullscreen)
{
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //DEBUG

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hwnd;								// the window to be used
	scd.SampleDesc.Count = m_sampleCount;                   // how many multisamples
	scd.Windowed = !fullscreen;							// windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,//D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_swapChain,
		&DX::g_device,
		NULL,
		&DX::g_deviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		// use the back buffer address to create the render target
		DX::g_device->CreateRenderTargetView(pBackBuffer, NULL, &m_backBufferRTV);
		//we are creating the standard depth buffer here.
		//_createDepthBuffer();

		DX::g_deviceContext->OMSetRenderTargets(1, &m_backBufferRTV, m_depthStencilView);	//As a standard we set the rendertarget. But it will be changed in the prepareGeoPass
		pBackBuffer->Release();
	}
	return hr;
}

void Engine3D::Release()
{
	DX::SafeRelease(DX::g_device);
	DX::SafeRelease(DX::g_deviceContext);
	DX::SafeRelease(m_swapChain);
	DX::SafeRelease(m_backBufferRTV);
	DX::SafeRelease(m_depthStencilView);
	DX::SafeRelease(m_depthBufferTex);
	DX::SafeRelease(m_samplerState);	
}
