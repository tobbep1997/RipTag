#pragma once
#include <Windows.h>
#include "../Engine/Source/Window/window.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../Engine/Source/3D Engine/Extern.h"
#include "../FunctionVector.h"

#define DIV 1048576
#define WIDTH 7

class ImGuiManager
{
private:
	MEMORYSTATUSEX m_statex;
	FunctionPush::Vector<ImGuiManager, void> m_ImGuiDrawVector;
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
