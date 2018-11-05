#pragma once
#include <Windows.h>

struct ProcMsg;

class Window;
class Engine3D;
class Camera;
class ImGuiManager;

class RenderingManager
{
private:
	bool	DEBUG = false;
	Window*		m_wnd;
	Engine3D*	m_engine;

	//IMGUI onlt take 1 mb in debug mode
	ImGuiManager * m_ImGuiManager;
public:
	RenderingManager();
	~RenderingManager();

	static RenderingManager * GetInstance();

	void Init(HINSTANCE hInstance);
	void Update();
	void UpdateSingleThread();

	void Clear();
	void Flush(Camera & camera);

	void Release();

	//Starts the new frame for imgui
	void ImGuiStartFrame();

	//---------------------------------
	//Explicit window related function
	Window& getWindow();
	ProcMsg getWindowProcMsg();

	void ImGuiProc();

private:
	void _reloadShaders();
};

