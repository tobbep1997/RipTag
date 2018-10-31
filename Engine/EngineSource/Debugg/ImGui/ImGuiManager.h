#pragma once
#include <Windows.h>
#include "../../Window/window.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../../3D Engine/Extern.h"
#include "../FunctionVector.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>

#pragma comment(lib, "dxgi.lib")

#include <psapi.h>

#include <string>

#define DIV 1048576
#define WIDTH 7

class ImGuiManager
{
private:
	MEMORYSTATUSEX m_statex;
	FunctionPush::Vector<ImGuiManager, void> m_ImGuiDrawVector;
	DWORD m_currentProcessID;
	HANDLE hProcess;
	float memoryUsageRam = 0;
	float memoryUsageVRam = 0;

	HRESULT ret_code;
	IDXGIFactory* dxgifactory = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIAdapter4* dxgiAdapter4 = NULL;
public:
	ImGuiManager();
	~ImGuiManager();
	void Release();

	//Start IMGUI
	void Init(HWND & hwnd);

	void StartFrame();
	//This will be the ImGui DrawThingy, give it to the rendering queue
	void Draw();

	void ImGuiProcPoll(ProcMsg & im);
private:
	void _MemoryManager();
};
