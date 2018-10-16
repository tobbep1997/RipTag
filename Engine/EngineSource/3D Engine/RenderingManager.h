#pragma once
#include "../Window/window.h"
#include "../Debugg/ImGui/ImGuiManager.h"
#include "3DRendering/Framework/Engine3D.h"



class RenderingManager
{
private:
	bool	DEBUG = false;
	Window		m_wnd;
	Engine3D	m_engine;

	//IMGUI onlt take 1 mb in debug mode
	ImGuiManager m_ImGuiManager;
public:
	RenderingManager();
	~RenderingManager();

	static RenderingManager * GetInstance();

	void Init(HINSTANCE hInstance);
	void Update();

	void Clear();
	void Flush(Camera & camera);

	void Release();

	//Starts the new frame for imgui
	void ImGuiStartFrame();

	//---------------------------------
	//Explicit window related function
	Window& getWindow();
	ProcMsg getWindowProcMsg();

private:
	void _reloadShaders();
};

