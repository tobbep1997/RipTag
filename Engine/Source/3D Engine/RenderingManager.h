#pragma once
#include "../Window/window.h"
#include "Engine3D.h"
#include "../Debugg/ImGui/ImGuiManager.h"



class RenderingManager
{
private:
	bool DEBUG = false;
	Window wnd;
	Engine3D engine;

	//IMGUI onlt take 1 mb in debug mode
	ImGuiManager m_ImGuiManager;
public:
	RenderingManager();
	~RenderingManager();

	void Init(HINSTANCE hInstance);
	void Update();

	void Flush(Camera & camera);

	void Release();

	//Starts the new frame for imgui
	void ImGuiStartFrame();

	//---------------------------------
	//Explicit window related function
	Window& getWindow();
	ProcMsg getWindowProcMsg();


};

