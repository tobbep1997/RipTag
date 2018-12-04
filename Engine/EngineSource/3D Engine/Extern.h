#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <vector>


class StaticMesh;

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
class ParticleEmitter;
class Camera;


namespace DX {
	extern ID3D11Device*			                                     g_device;
	extern ID3D11DeviceContext1*	                             g_deviceContext;
	extern void SafeRelease(IUnknown * u);
	extern Shaders::ShaderManager                              g_shaderManager;

	extern Drawable*                                                     g_player;
	extern Drawable*                                                     g_remotePlayer;
	extern std::vector<Drawable*>                               g_animatedGeometryQueue;
	extern std::vector<Drawable*>                               g_outlineQueue;
	extern std::vector<Drawable*>                               g_wireFrameDrawQueue;

	extern std::vector<PointLight*>                             g_lights;
	extern std::vector<PointLight*>                             g_prevlights;
	extern std::vector<VisibilityComponent*>             g_visibilityComponentQueue;

	extern std::vector<Quad*>                                     g_2DQueue;

	extern std::vector<ParticleEmitter*>                      g_emitters;
	extern std::vector <Drawable*>                             g_cullQueue;

	extern RECT                                                            g_backBufferResolution;

	extern bool g_screenShootCamera;

	namespace INSTANCING
	{
		
		struct OBJECT
		{
			DirectX::XMFLOAT4X4A worldMatrix;
			DirectX::XMFLOAT4A objectColor;
			DirectX::XMFLOAT4A textureTileMult;
			DirectX::XMINT4 usingTexture;
		};
		struct GROUP
		{
			std::vector<OBJECT> attribs;
			StaticMesh* staticMesh;
			std::string textureName;
		};
		struct GROUP_INDEXED
		{
			std::vector<OBJECT> attribs;
			Drawable* drawable;
			std::vector<long> index;
		};

		extern std::vector<GROUP>         g_instanceGroups;
		extern std::vector<GROUP>         g_instanceWireFrameGroups;
		extern std::vector<GROUP>         g_instanceShadowGroups;

		extern void submitToShadowQueueInstance(Drawable* drawable);
		extern void submitToWireframeInstance(Drawable* drawable);
		extern void submitToInstance(Drawable* drawable, Camera * camera);

		extern std::vector<GROUP> g_temp;
		extern void tempInstance(Drawable* drawable);
	}

}
namespace Manager
{
	extern MeshManager            g_meshManager;
	extern TextureManager         g_textureManager;
	extern AnimationHandler      g_animationManager;
}
