#include "Window.h"
#include "Winuser.h"

/// <summary>
/// Next available HMENU ID
/// </summary>
HMENU Window::nextId = (HMENU)1;

// Creates a window using a specific width, height,
// location, and title.
//
Window::Window(HWND parent, u16 width, u16 height, u16 x, u16 y, const wchar_t* title, const wchar_t* _class)
	: width{ width }, height{ height }, x{ x }, y{ y }, parent{ parent }, ps{ 0 }
{
	const HINSTANCE hInstance = GetModuleHandle(NULL);

	u64 mask = (u64)(!parent) - 1ll;

	CreateWindowEx(0, _class, title, (~mask & (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX)) | (mask & WS_CHILD),
		x, y, width, height, parent, NULL, hInstance, this);
}

// Creates a window using a specific width, height,
// location, and title.
//
Window::Window(HWND parent, HINSTANCE hInstance, u16 width, u16 height, u16 x, u16 y, HMENU id, int extraFlags, const wchar_t* title, const wchar_t* _class, WIN32_EVENT(onCreate))
	: width{ width }, height{ height }, x{ x }, y{ y }, parent{ parent }, ps{ 0 }, onCreate{ onCreate }
{
	u64 mask = (u64)(!parent) - 1ll;

	CreateWindowEx(0, _class, title, (~mask & (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX)) | (mask & WS_CHILD) | extraFlags,
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
void Window::RegisterButton(Button button, WIN32_EVENT(onClick))
{
	HWND btn = CreateWindowEx(0, TEXT("BUTTON"), button.text,
					WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
					button.x, button.y, button.w, button.h,
					this->handle, Window::nextId, GetModuleHandle(NULL), NULL);

	this->buttons.Add({ Window::nextId, onClick });
	SendMessage(btn, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

	Window::nextId = (HMENU)((long long)Window::nextId + 1);
}

// Create label
//
HWND Window::RegisterLabel(Label label, TextAlignment align)
{
	HWND hLabel = CreateWindowEx(0, TEXT("STATIC"), label.text,
					 WS_VISIBLE | WS_CHILD | (long)align,
					 label.x, label.y, label.w, label.h,
					 this->handle, (HMENU)0, GetModuleHandle(NULL), NULL);

	SendMessage(hLabel, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
	return hLabel;
}

// Create textbox
//
void Window::RegisterTextBox(TextBox textBox, WIN32_EVENT(onInput))
{
	HWND txt = CreateWindowEx(0, TEXT("EDIT"), textBox.text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | (long)textBox.type | WS_BORDER,
		textBox.x, textBox.y, textBox.w, textBox.h,
		this->handle, Window::nextId, GetModuleHandle(NULL), NULL);

	this->textboxes.Add({ Window::nextId, onInput });
	SendMessage(txt, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);

	Window::nextId = (HMENU)((long long)Window::nextId + 1);
}

// WinProc callback dispatcher function.
//
LRESULT Window::WndProcHandler(WNDPROC_ARGS)
{
	Window* app;

	if (uMsg == WM_CREATE)
	{
		app = (Window*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		app->handle = hwnd;
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
	LRESULT lr;

	switch (uMsg)
	{
	// Creation event
	case WM_CREATE:
		return this->onCreate(WNDPROC_ARGS_UNPACK);

	// Paint event
	case WM_PAINT:
		this->BeginPaint();
		lr = this->onPaint(WNDPROC_ARGS_UNPACK);
		this->EndPaint();
		return lr;

	// The window is getting resized.
	case WM_SIZE:
		this->width = LOWORD(lParam);
		this->height = HIWORD(lParam);
		return this->onResize(WNDPROC_ARGS_UNPACK);
		
	// The window has moved.
	case WM_MOVING:
	{
		RECT* pos = (RECT*)lParam;
		this->x = (u16)pos->left;
		this->y = (u16)pos->top;
		return this->onMove(WNDPROC_ARGS_UNPACK);
	}

	// Command received (generally control input).
	case WM_COMMAND:
		for (int i = 0; i < this->buttons.GetLength(); i += 1)
		{
			if ((HMENU)LOWORD(wParam) == this->buttons[i].menuID)
			{
				this->buttons[i].onClick(WNDPROC_ARGS_UNPACK);
			}
		}
		for (int i = 0; i < this->textboxes.GetLength(); i += 1)
		{
			if ((HMENU)LOWORD(wParam) == this->textboxes[i].menuID && HIWORD(wParam) == EN_UPDATE)
			{
				this->textboxes[i].onInput((HWND)lParam, EN_UPDATE, wParam, lParam);
			}

		}
		return this->onCommand(WNDPROC_ARGS_UNPACK);
		
	// The window's close button was pressed.
	case WM_CLOSE:
		lr = this->onClose(WNDPROC_ARGS_UNPACK);
		DestroyWindow(this->handle);
		return lr;
		
	// The window is getting destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(WNDPROC_ARGS_UNPACK);
		break;
	}

	return 0;
}

// Registers a new class
//
bool __fastcall Window::CreateClass(const wchar_t* className, HBRUSH background, WNDPROC wndProc)
{
	const HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wndc = { 0 };
	wndc.lpszClassName = className;
	wndc.hInstance = hInstance;
	wndc.lpfnWndProc = wndProc;
	wndc.hbrBackground = background;

	return RegisterClass(&wndc);
}
