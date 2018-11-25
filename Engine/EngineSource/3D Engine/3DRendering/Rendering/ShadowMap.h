#pragma once
#pragma warning (disable : 4267)
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

namespace Animation
{
	class AnimationCBuffer;
};
class Drawable;
class PointLight;
class Camera;

class ShadowMap
{
private:

struct ObjectBuffer
{
	DirectX::XMFLOAT4X4A worldMatrix;
};
struct PointLightBuffer
{
	DirectX::XMFLOAT4X4A viewProjection[8][6];
	DirectX::XMINT4 nrOfviewProjection[8];
	DirectX::XMINT4 nrOfLights;
	DirectX::XMUINT4 useDir[8][6];

};

struct LightIndex
{
	DirectX::XMUINT4 lightPos;
	DirectX::XMUINT4 useSides[8];
};


private:
	const unsigned int RENDER_TARGET_VIEW_COUNT = 6;
	const unsigned int SHADER_RESOURCE_VIEW_COUNT = 8*6;

	D3D11_VIEWPORT				m_shadowViewport;
	ID3D11SamplerState*			m_shadowSamplerState;
	ID3D11ShaderResourceView *	m_shadowShaderResourceView;
	ID3D11DepthStencilView*		m_shadowDepthStencilView;
	ID3D11Texture2D*			m_shadowDepthBufferTex;
	

	ID3D11RenderTargetView*		m_renderTargetView;
	ID3D11Texture2D*			m_renderTargetsTexture;

	ID3D11Buffer* m_objectBuffer = nullptr;
	ObjectBuffer m_objectValues;

	ID3D11Buffer * m_allLightMatrixBuffer = nullptr;
	PointLightBuffer m_allLightMatrixValues;

	ID3D11Buffer * m_lightIndexBuffer = nullptr;
	LightIndex m_lightIndex;

	int m_runned = 0;

	unsigned int m_currentLight = 0;

public:
	ShadowMap();
	~ShadowMap();

	void Init(UINT width, UINT height);

	void ShadowPass(ForwardRender * renderingManager);

	void MapAllLightMatrix(std::vector<PointLight*> * lights);

	void SetSamplerAndShaderResources();
	void Clear();
	void Release();
private:
	void _createShadowViewPort(UINT width, UINT height);
	void _createShadowDepthStencilView(UINT width, UINT hight);
	void _createBuffers();
	void _createRenderTargets(UINT width, UINT height);
	void _mapSkinningBuffer(Drawable * d, Animation::AnimationCBuffer * animBuffer);
	void _mapObjectBuffer(Drawable * drawable);
};

