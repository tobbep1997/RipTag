#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <vector>

//#include "Model/Managers/MeshManager.h"
//#include "Model/Managers/TextureManager.h"


namespace Shaders
{
	class ShaderManager;
}

class Drawable;
class VisibilityComponent;
class Quad;
class PointLight;
class MeshManager;
class TextureManager;
class AnimationHandler;

namespace DX {
	extern ID3D11Device*			g_device;
	extern ID3D11DeviceContext1*	g_deviceContext;
	extern void SafeRelease(IUnknown * u);
	extern Shaders::ShaderManager g_shaderManager;

	extern std::vector<Drawable*> g_geometryQueue;
	extern std::vector<Drawable*> g_animatedGeometryQueue;
	extern std::vector<Drawable*> g_visabilityDrawQueue;
	extern std::vector<Drawable*> g_wireFrameDrawQueue;

	extern std::vector<PointLight*> g_lights;
	extern std::vector<VisibilityComponent*> g_visibilityComponentQueue;

	extern std::vector<Quad*> g_2DQueue;

}
namespace Manager
{
	extern MeshManager g_meshManager;
	extern TextureManager g_textureManager;
	extern AnimationHandler g_animationManager;
}
