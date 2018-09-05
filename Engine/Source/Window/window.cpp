#include "window.h"

namespace
{
	Window* g_wndPtr = nullptr; 
}

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

LRESULT Window::msgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		m_windowContext.clientHeight = HIWORD(lParam);
		m_windowContext.clientWidth = LOWORD(lParam);
		return 0;

	default:
		DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

LRESULT CALLBACK MainWindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (g_wndPtr) return g_wndPtr->msgProc(hwnd, msg, wParam, lParam);
	else return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Window()
{
	m_windowContext.posX = 50; 
	m_windowContext.posY = 50; 
	m_windowContext.clientHeight = 100; 
	m_windowContext.clientWidth = 100; 
	m_windowContext.windowTitle = "Hahah"; 
}

Window::~Window()
{
}

bool Window::Init(window::WindowContext windowContext)
{
	ZeroMemory(&m_windowContext.wcex, sizeof(WNDCLASSEX)); 
	m_windowContext.wcex.cbClsExtra = 0; 
	m_windowContext.wcex.cbWndExtra = 0; 
	m_windowContext.wcex.cbSize = sizeof(WNDCLASSEX); 
	m_windowContext.wcex.style = CS_HREDRAW | CS_VREDRAW;
	m_windowContext.wcex.hInstance = m_windowContext.windowInstance; //Member in window instead? (windowInstance)
	m_windowContext.wcex.lpfnWndProc = MainWindProc;
	m_windowContext.wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	m_windowContext.wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 
	m_windowContext.wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	m_windowContext.wcex.lpszMenuName = NULL;
	m_windowContext.wcex.lpszClassName = "WNDCLASS"; 
	m_windowContext.wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION); 

	if (!RegisterClassEx(&m_windowContext.wcex))
	{
		OutputDebugString("FAILED TO CREATE WINDOW CLASS!\n");
		return false; 
	}

	RECT r = { 0, 0, m_windowContext.clientWidth, m_windowContext.clientHeight };
	AdjustWindowRect(&r, m_windowContext.wcex.style, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	//m_windowContext.posX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	//m_windowContext.posY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;


	m_wHandler = CreateWindow("WNDCLASS","Example Title" , m_windowContext.wcex.style,
		m_windowContext.posX, m_windowContext.posY, width, height, NULL, NULL, m_windowContext.windowInstance, NULL);

	if (!m_wHandler)
	{
		OutputDebugString("FAILED TO CREATE WINDOW\n");
		return false;
	}

	return true;
}

void Window::Update()
{
}

window::WindowContext& Window::getWindowContext()
{
	return m_windowContext; 
}

HWND & Window::getHandler()
{
	return m_wHandler; 
}
