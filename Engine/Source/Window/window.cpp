#include "window.h"
#include "../Debug/ImGui/imgui.h"

void Window::_resize(UINT width, UINT height)
{
	m_windowContext.clientWidth = width;
	m_windowContext.clientHeight = height;
	SetWindowPos(m_wHandler, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);
}

void Window::_setPosition(UINT posX, UINT posY)
{
	SetWindowPos(m_wHandler, HWND_TOP, posX, posY, m_windowContext.clientWidth, m_windowContext.clientHeight, SWP_SHOWWINDOW);
}

void Window::_toggleFullScreen()
{
	//TODO: Sending the fullscreen comand to the right place for.
	//changing of the fullscreen
	if (!m_windowContext.fullscreen)
	{
		m_windowContext.fullscreen = true;
	}
	else
	{
		m_windowContext.fullscreen = false; 
	}
}

void Window::_destroy()
{
}

LRESULT Window::StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* pParent;
	if (uMsg == WM_CREATE)
	{
		pParent = (Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pParent);
	}
	else
	{
		pParent = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!pParent) return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	pParent->m_wHandler = hWnd;
	return pParent->MsgProc(hWnd, uMsg, wParam, lParam);
}


LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE: //If user change the window size
		m_windowContext.clientHeight = HIWORD(lParam);
		m_windowContext.clientWidth = LOWORD(lParam);
		break;
	}
	
	m_procMsg.hwnd = hwnd;
	m_procMsg.msg = msg;
	m_procMsg.wParam = wParam;
	m_procMsg.lParam = lParam;

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

ProcMsg & Window::getWindowProcMsg()
{
	return m_procMsg;
}


Window::Window()
{
	
}

Window::~Window()
{
}

bool Window::Init(WindowContext windowContext)
{
	m_windowContext = windowContext;

	ZeroMemory(&m_windowContext.wcex, sizeof(WNDCLASSEX)); 
	m_windowContext.wcex.cbClsExtra = 0; 
	m_windowContext.wcex.cbWndExtra = 0; 
	m_windowContext.wcex.cbSize = sizeof(WNDCLASSEX); 
	m_windowContext.wcex.style = CS_HREDRAW | CS_VREDRAW;
	m_windowContext.wcex.hInstance = m_windowContext.windowInstance; //Member in window instead? (windowInstance)
	m_windowContext.wcex.lpfnWndProc = StaticWndProc;
	m_windowContext.wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	m_windowContext.wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 
	m_windowContext.wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	m_windowContext.wcex.lpszMenuName = NULL;
	m_windowContext.wcex.lpszClassName = L"WNDCLASS"; 
	m_windowContext.wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 

	if (!RegisterClassEx(&m_windowContext.wcex))
	{
		OutputDebugString(L"FAILED TO CREATE WINDOW CLASS!\n");
		return false; 
	}
	
	RECT r = { 0, 0, m_windowContext.clientWidth, m_windowContext.clientHeight };
	AdjustWindowRect(&r, m_windowContext.wcex.style, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	m_wHandler = CreateWindow(
		m_windowContext.wcex.lpszClassName,
		m_windowContext.windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		nullptr,
		nullptr,
		m_windowContext.windowInstance,
		this);

	if (!m_wHandler)
	{
		OutputDebugString(L"FAILED TO CREATE WINDOW\n");
		return false;
	}

	ShowWindow(m_wHandler, 10);
	
	return true;
}

void Window::Update(CoreMessage msg)
{
}

void Window::PollEvents()
{
	if (PeekMessage(&m_Peekmsg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_Peekmsg);
		DispatchMessage(&m_Peekmsg);
	}
}

bool Window::isOpen()
{
	return WM_QUIT != m_Peekmsg.message;
}

WindowContext& Window::getWindowContext()
{
	return m_windowContext; 
}

HWND & Window::getHandler()
{
	return m_wHandler; 
}
