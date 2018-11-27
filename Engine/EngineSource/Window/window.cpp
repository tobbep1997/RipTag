#include "EnginePCH.h"
#include "window.h"


//InputHandler::InputHandler& Instance();

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
	bool GainedFocus = false;

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE: //If user change the window size
		m_windowContext.clientHeight = HIWORD(lParam);
		m_windowContext.clientWidth = LOWORD(lParam);
		break;

	//KEYBOARD INPUT

	case WM_KEYDOWN:
		InputHandler::m_keys[wParam] = true;
		InputHandler::m_lastPressed = static_cast<int>(wParam);
		InputHandler::m_rawInput.push_back(static_cast<unsigned int>(wParam));
		break; 

	case WM_KEYUP:
		InputHandler::m_keysReleased[wParam] = true;
		InputHandler::m_keysPressed[wParam] = true;
		InputHandler::m_keys[wParam] = false;
		InputHandler::m_lastPressed = -1; 
		break; 
	//MOUSE INPUT

		//Left MB
	case WM_LBUTTONDOWN:
		InputHandler::m_mouseKeys[0] = true; 
		break;
	case WM_LBUTTONUP:
		InputHandler::m_mouseKeys[0] = false; 
		break; 

		//Middle MB
	case WM_MBUTTONDOWN:
		InputHandler::m_mouseKeys[1] = true;
		break; 

	case WM_MBUTTONUP: 
		InputHandler::m_mouseKeys[1] = false;
		break; 

		//Right MB 
	case WM_RBUTTONDOWN:
		InputHandler::m_mouseKeys[2] = true; 
		break; 
	case WM_RBUTTONUP:
		InputHandler::m_mouseKeys[2] = false; 
		break;

	case WM_MOUSEMOVE:
		InputHandler::m_mousePos.x = LOWORD(lParam); 
		InputHandler::m_mousePos.y = HIWORD(lParam); 
		InputHandler::m_mouseMoved = true;
		break;

		//1 or -1
	case WM_MOUSEWHEEL:
		InputHandler::m_scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120.0f; 
		break;

	case WM_KILLFOCUS:
		InputHandler::m_windowInFocus = false;
		break;
	case WM_SETFOCUS:
		if (!InputHandler::m_windowInFocus)
			GainedFocus = true;
		InputHandler::m_windowInFocus = true;
		break;
	}
	RECT Rect;
	GetWindowRect(hwnd, &Rect);
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&Rect, 2);
	InputHandler::m_windowSize = { (INT) m_windowContext.clientWidth, (INT) m_windowContext.clientHeight };
	InputHandler::m_windowPos = DirectX::XMFLOAT2((float)Rect.left, (float)Rect.top);
	InputHandler::m_viewportPos = DirectX::XMFLOAT2((float)Rect.left + (!m_windowContext.fullscreen * 8.f), (float)Rect.top + (!m_windowContext.fullscreen * 31.f));
	if (GainedFocus)
	{
		SetCursorPos(static_cast<int>(InputHandler::m_viewportPos.x + m_windowContext.clientWidth / 2), static_cast<int>(InputHandler::m_viewportPos.y + m_windowContext.clientHeight / 2));
		InputHandler::m_mousePos.x = m_windowContext.clientWidth / 2.0f;
		InputHandler::m_mousePos.y = m_windowContext.clientHeight / 2.0f;
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
	m_isOpen = true;
}

Window::~Window()
{
}

bool Window::Init(_In_ WindowContext windowContext)
{
	m_windowContext = windowContext;
	static bool firstRun = true;
	if (!firstRun)
	{
		DestroyWindow(m_wHandler);
		BOOL lol = UnregisterClass(L"WNDCLASS", windowContext.windowInstance);
		int i = 0;
	}

	
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
	

	InputHandler::Instance();
	InputHandler::m_viewportSize = { (INT) m_windowContext.clientWidth, (INT) m_windowContext.clientHeight };
	//Give InputHandler neccesary dimension information 
	InputHandler::m_windowSize.x = m_windowContext.clientWidth;
	InputHandler::m_windowSize.y = m_windowContext.clientHeight; 
	for (int i = 0; i < 256; i++)
	{
		InputHandler::m_keys[i]			= false;
		InputHandler::m_keysReleased[i]	= false;
		InputHandler::m_keysPressed[i]	= true;
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
		r.right - r.left + (!m_windowContext.fullscreen * 16),
		r.bottom - r.top + (!m_windowContext.fullscreen * 39),
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
	firstRun = false;
	return true;
}

//void Window::Update(CoreMessage msg)
//{
//}

void Window::PollEvents()
{
	while (PeekMessage(&m_Peekmsg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_Peekmsg);
		DispatchMessage(&m_Peekmsg);
	}
}

bool Window::isOpen()
{
	if (InputHandler::GetClosedGame())
	{
		return false;
	}
	else
	{
		return WM_QUIT != m_Peekmsg.message;
	}
	
}

void Window::Close()
{
	m_isOpen = false;
}

WindowContext& Window::getWindowContext()
{
	return m_windowContext; 
}

HWND & Window::getHandler()
{
	return m_wHandler; 
}
