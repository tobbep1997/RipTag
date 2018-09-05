#include "window.h"


Window* g_wndPtr = nullptr; 


void Window::_resize()
{

}

void Window::_setPosition()
{
}

void Window::_toggleFullScreen()
{
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
	case WM_SIZE:
		m_windowContext.clientHeight = HIWORD(lParam);
		m_windowContext.clientWidth = LOWORD(lParam);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
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

	//m_windowContext.posX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	//m_windowContext.posY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;


	//m_wHandler = CreateWindow(L"WNDCLASS", L"Example Title" , m_windowContext.wcex.style,
		//m_windowContext.posX, m_windowContext.posY, width, height, NULL, NULL, m_windowContext.windowInstance, NULL);

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

void Window::Update()
{
}

void Window::PollEvents()
{
	if (PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}
}

bool Window::isOpen()
{
	return WM_QUIT != m_msg.message;
}

WindowContext& Window::getWindowContext()
{
	return m_windowContext; 
}

HWND & Window::getHandler()
{
	return m_wHandler; 
}
