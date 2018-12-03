#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "EngineSource/Debugg/ImGui/ImGuiManager.h"

namespace DirectX {
	class SpriteBatch;
};

class Quad;


class Render2D
{
private:
	enum HUDTypeEnum
	{
		QuadType = 0U,
		CircleType = 1U,
		OutlinedQuad = 2U
	};
	struct HUDTypeStruct
	{
		DirectX::XMFLOAT4A center;
		DirectX::XMFLOAT4A color;
		DirectX::XMFLOAT4A outlineColor;
		DirectX::XMUINT4 type;
	};
private:
	ID3D11SamplerState *	m_sampler;		//RELEASED
	DirectX::SpriteBatch *	m_spriteBatch;

	ID3D11BlendState *		m_blendState; //RELEASED
	ID3D11DepthStencilState*	m_depthStencilState; //RELEASED

	ID3D11Buffer * m_HUDTypeBuffer; //RELEASED
	HUDTypeStruct m_HUDTypeValues;
#ifndef _DEPLOY
	MEMORYSTATUSEX m_statex;

	DWORD m_currentProcessID;
	HANDLE hProcess;
	float memoryUsageRam = 0;
	float memoryUsageVRam = 0;

	Quad * dbg_quad = nullptr;

	HRESULT ret_code;
	IDXGIFactory* dxgifactory = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIAdapter4* dxgiAdapter4 = NULL;
#endif

public:
	Render2D();
	~Render2D();

	void Init();
	void GUIPass();
	void Release();

#ifndef _DEPLOY
	void DBG_INIT();
	void DBG();
	void DBG_RELEASE();
#endif
};

