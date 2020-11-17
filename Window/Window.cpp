#include "Window.h"

// Creates a window using a specific width, height,
// location, and title.
//
Window::Window(u16 width, u16 height, u16 x, u16 y, const wchar_t* title)
	: width{ width }, height{ height }, x{ x }, y{ y } 
{
	const wchar_t* className = L"WND_CLASS_GENERATED";
	const HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wndc = { 0 };
	wndc.lpszClassName = className;
	wndc.hInstance = hInstance;
	
	RegisterClass(&wndc);

	this->hWnd = CreateWindowEx(0, className, title, WS_OVERLAPPEDWINDOW,
		x, y, width, height, NULL, NULL, hInstance, NULL);

	SetWindowLongPtr(this->hWnd, GWLP_USERDATA, (LONG_PTR)this);
	ShowWindow(this->hWnd, false);
}

// Destructor
//
Window::~Window()
{
	DestroyWindow(this->hWnd);
}

// Retrieves the current size of this window.
//
Size<u16> Window::GetWindowSize() const
{
	return { this->width, this->height };
}

// Resizes the window to a specific size.
//
void Window::ResizeWindow(u16 width, u16 height)
{
	this->width = width;
	this->height = height;

	SetWindowPos(this->hWnd, HWND_TOP,
		this->x, this->y, width, height, SWP_NOZORDER | SWP_NOMOVE);
}

// WinProc callback dispatcher function.
//
LRESULT Window::WndProcHandler(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	Window* this_ptr = (Window*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return this_ptr->WindowProc(hwnd, uMsg, wParam, lParam);
}

// WinProc callback function.
//
LRESULT CALLBACK Window::WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	DefWindowProc(hWnd, uMsg, wParam, lParam);
}
