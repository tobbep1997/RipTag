#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class ForwardRender;
class Camera;

class Engine3D
{
private:
	IDXGISwapChain*				m_swapChain;
	ID3D11RenderTargetView*		m_backBufferRTV;
	ID3D11DepthStencilView*		m_depthStencilView;
	ID3D11Texture2D*			m_depthBufferTex;
	ID3D11SamplerState*			m_samplerState;
	ID3D11DepthStencilState*	m_depthStencilState;
	D3D11_VIEWPORT				m_viewport;


	ForwardRender*				m_forwardRendering;


	UINT m_sampleCount = 1;
	
public:
	Engine3D();
	~Engine3D();

	HRESULT Init(HWND hwnd, bool fullscreen, UINT width = 0, UINT hight = 0);

	void Flush(Camera & camera, ParticleEmitter * lol);
	void Clear();
	void Present();
	
	void Release();
private:
	void _createDepthSetencil(UINT width = 0, UINT hight = 0);
	void _initViewPort(UINT width = 0, UINT hight = 0);
};


