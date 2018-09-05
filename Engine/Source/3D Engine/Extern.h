#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace DX {
	extern ID3D11Device*			g_device;
	extern ID3D11DeviceContext*		g_deviceContext;
	   
	extern void SafeRelease(IUnknown * u);
}
