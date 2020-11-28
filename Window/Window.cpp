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
	wndc.lpfnWndProc = Window::WndProcHandler;
	
	RegisterClass(&wndc);

	this->handle = CreateWindowEx(0, className, title, WS_OVERLAPPEDWINDOW,
		x, y, width, height, NULL, NULL, hInstance, this);
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

// Polls the window for a message and returns it to be
// handled.
//
bool Window::PollMessage(UINT* oMsg)
{
	MSG msg;
	bool msgGot;

	if (msgGot = GetMessage(&msg, this->handle, NULL, 0) > 0)
	{
		switch (msg.message)
		{
			// The window is getting resized.
			case WM_SIZE:
				this->width = LOWORD(msg.lParam);
				this->height = HIWORD(msg.lParam);
				break;

			// The window has moved.
			case WM_MOVING:
			{
				RECT* pos = (RECT*)msg.lParam;
				this->x = (u16)pos->left;
				this->y = (u16)pos->top;
				break;
			}

			// The window's close button was pressed.
			case WM_QUIT:
				DestroyWindow(this->handle);
				break;

			// The window is getting destroyed.
			case WM_DESTROY:
				PostQuitMessage(0);
				break;

		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		*oMsg = msg.message;
	}

	return msgGot;
}

// WinProc callback dispatcher function.
//
LRESULT Window::WndProcHandler(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	//Window* this_ptr;

	//switch (uMsg)
	//{
	//case WM_CREATE:
	//	this_ptr = (Window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
	//	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this_ptr);
	//	break;

	//default:
	//	this_ptr = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	//	break;

	//}

	//// Since the order of messages is a mess
	//if (this_ptr)
	//{
	//	switch (uMsg)
	//	{
	//	case WM_SIZE:
	//		this_ptr->width = LOWORD(lParam);
	//		this_ptr->height = HIWORD(lParam);
	//		break;

	//	case WM_MOVING:
	//	{
	//		RECT* pos = (RECT*)lParam;
	//		this_ptr->x = (u16)pos->left;
	//		this_ptr->y = (u16)pos->top;
	//		break;
	//	}

	//	case WM_QUIT:
	//		this_ptr->shouldClose = true;
	//		DestroyWindow(hwnd);
	//		break;

	//	case WM_DESTROY:
	//		PostQuitMessage(0);
	//		break;

	//	}
	//}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}