#include "Window.h"
#include "Winuser.h"

// Creates a window using a specific width, height,
// location, and title.
//
Window::Window(HWND parent, u16 width, u16 height, u16 x, u16 y, const wchar_t* title, const wchar_t* _class)
	: width{ width }, height{ height }, x{ x }, y{ y }, parent{ parent }, ps{ 0 }
{
	const HINSTANCE hInstance = GetModuleHandle(NULL);

	u64 mask = (u64)(!parent) - 1ll;

	this->handle = CreateWindowEx(0, _class, title, (~mask & WS_OVERLAPPEDWINDOW) | (mask & WS_CHILD),
		x, y, width, height, parent, NULL, hInstance, this);
}

// Creates a window using a specific width, height,
// location, and title.
//
Window::Window(HWND parent, HINSTANCE hInstance, u16 width, u16 height, u16 x, u16 y, HMENU id, int extraFlags, const wchar_t* title, const wchar_t* _class, WIN32_EVENT(onCreate))
	: width{ width }, height{ height }, x{ x }, y{ y }, parent{ parent }, ps{ 0 }, onCreate{ onCreate }
{
	u64 mask = (u64)(!parent) - 1ll;

	this->handle = CreateWindowEx(0, _class, title, (~mask & WS_OVERLAPPEDWINDOW) | (mask & WS_CHILD) | extraFlags,
		x, y, width, height, parent, id, hInstance, this);
}

// Destructor
//
Window::~Window()
{
	DestroyWindow(this->handle);
}

// Resizes the window to a specific size.
//
void Window::ResizeWindow(u16 width, u16 height)
{
	this->width = width;
	this->height = height;

	SetWindowPos(this->handle, HWND_TOP,
		this->x, this->y, width, height, SWP_NOZORDER | SWP_NOMOVE);
}

// Create button
//
void Window::DefineButton(BUTTONREF button)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	CreateWindowEx(0, TEXT("BUTTON"), button.text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		button.x, button.y, button.w, button.h,
		this->handle, button.id, hInstance, NULL);
}

// WinProc callback dispatcher function.
//
LRESULT Window::WndProcHandler(WNDPROC_ARGS)
{
	Window* app;

	if (uMsg == WM_CREATE)
	{
		app = (Window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)app);
	}
	else
	{
		app = GRAB_WINDOW();
	}

	return app->WndProc(hwnd, uMsg, wParam, lParam);
}

// Actual window procedure
//
LRESULT Window::WndProc(WNDPROC_ARGS)
{
	switch (uMsg)
	{
	// Creation event
	case WM_CREATE:
		this->onCreate(hwnd, uMsg, wParam, lParam);
		break;

	// Paint event
	case WM_PAINT:
		this->BeginPaint();
		this->onPaint(hwnd, uMsg, wParam, lParam);
		this->EndPaint();
		break;

	// The window is getting resized.
	case WM_SIZE:
		this->width = LOWORD(lParam);
		this->height = HIWORD(lParam);
		break;
		
	// The window has moved.
	case WM_MOVING:
	{
		RECT* pos = (RECT*)lParam;
		this->x = (u16)pos->left;
		this->y = (u16)pos->top;
		break;
	}
		
	// The window's close button was pressed.
	case WM_CLOSE:
		DestroyWindow(this->handle);
		break;
		
	// The window is getting destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

// Registers a new class
//
bool __fastcall Window::CreateClass(const wchar_t* className, COLORREF background, WNDPROC wndProc)
{
	const HINSTANCE hInstance = GetModuleHandle(NULL);
	HBRUSH brush = CreateSolidBrush(background);

	WNDCLASS wndc = { 0 };
	wndc.lpszClassName = className;
	wndc.hInstance = hInstance;
	wndc.lpfnWndProc = wndProc;
	wndc.hbrBackground = brush;
	
	DeleteObject(brush);

	return RegisterClass(&wndc);
}
