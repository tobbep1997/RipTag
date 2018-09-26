#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#include <vector>
//#include "../Shader/ShaderManager.h"
#include "Model/Drawable.h"
#include "../Light/PointLight.h"
#include "Temp_Guard/TempGuard.h"
namespace Shaders
{
	class ShaderManager;
}

class Drawable;
class Guard;

namespace DX {
	extern ID3D11Device*			g_device;
	extern ID3D11DeviceContext1*	g_deviceContext;
	   
	extern void SafeRelease(IUnknown * u);
	extern Shaders::ShaderManager g_shaderManager;

	extern std::vector<Drawable*> g_geometryQueue;
	extern std::vector<Drawable*> g_animatedGeometryQueue;
	extern std::vector<PointLight*> g_lights;

	//Might be changed
	extern std::vector<Drawable*> g_visabilityDrawQueue;

	//TODO:: TEMP AFFF
	extern std::vector<Guardd*> g_guardDrawQueue;

}
