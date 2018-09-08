#pragma once
#include "../Window/window.h"
#include "Engine3D.h"
#include "../Debug/ImGuiManager.h"



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

	void Flush();

	void Release();

	

	//---------------------------------
	//Explicit window related function
	Window& getWindow();
	ProcMsg getWindowProcMsg();


};

