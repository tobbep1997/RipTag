#pragma once
#include <Windows.h>

struct ProcMsg;
struct WindowContext;

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
	HINSTANCE m_hInstance;
	WindowContext * m_wind;
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

	//---------------------------------
	//Explicit window related function
	Window& getWindow();
	ProcMsg getWindowProcMsg();

	void ImGuiProc();

	void Reset();

private:
	void _reloadShaders();
};

