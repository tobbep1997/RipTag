#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <vector>

#include "../Light/PointLight.h"
#include "Components/Base/Drawable.h"
#include "Model/Managers/MeshManager.h"
#include "Model/Managers/TextureManager.h"
#include "3DRendering/Rendering/VisabilityPass/Component/VisibilityComponent.h"

namespace Shaders
{
	class ShaderManager;
}

class Drawable;
class VisibilityComponent;
//class Guard;

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

	extern std::vector<Drawable*> g_wireFrameDrawQueue;

	//TODO:: TEMP AFFF
	extern std::vector<VisibilityComponent*> g_visibilityComponentQueue;

}
//Tesla was here
namespace Manager
{
	extern MeshManager g_meshManager;
	extern TextureManager g_textureManager;
}
