#include "Window.h"

// Creates a window using a specific width, height,
// location, and title.
//
Window::Window(HWND parent, u16 width, u16 height, u16 x, u16 y, const wchar_t* title)
	: width{ width }, height{ height }, x{ x }, y{ y }, parent{ parent }
{
	const wchar_t* className = L"WND_CLASS_GENERATED";
	const HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wndc = { 0 };
	wndc.lpszClassName = className;
	wndc.hInstance = hInstance;
	wndc.lpfnWndProc = Window::WndProcHandler;
	
	RegisterClass(&wndc);

	u64 mask = (u64)(!parent) - 1ll;

	this->handle = CreateWindowEx(0, className, title, (~mask & WS_OVERLAPPEDWINDOW) | (mask & WS_CHILD),
		x, y, width, height, parent, NULL, hInstance, this);
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
bool Window::PollMessage(MSG* oMsg)
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

		*oMsg = msg;
	}

	return msgGot;
}

// Peeks at the next message in the message queue.
//
MSG Window::NextMessage()
{
	MSG msg;

	PeekMessage(&msg, this->handle, 0, 0, true);

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

	return msg;
}

// WinProc callback dispatcher function.
//
LRESULT Window::WndProcHandler(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}