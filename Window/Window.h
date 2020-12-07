#pragma once
#include <Windows.h>
#include "DataStructures.h"

#define WNDPROC_ARGS _In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam
#define GRAB_WINDOW() (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA)

#define GET_HINSTANCE() ((LPCREATESTRUCT)lParam)->hInstance
#define WIN32_EVENT(x) LRESULT(*x)(WNDPROC_ARGS)
#define WIN32_DEFAULT_EVENT_HANDLER DefWindowProc

#define BUTTON(...)								{ __VA_ARGS__ }
#define REGISTER_BUTTON(hwnd, hinstance, btn)	{																	\
													CreateWindowEx(0, TEXT("BUTTON"), btn.text,						\
													WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,				\
													btn.x, btn.y, btn.w, btn.h, hwnd, btn.id, hinstance, NULL);		\
												}

/// <summary>
/// Defines a button.
/// </summary>
struct BUTTONREF
{
	u16 x;
	u16 y;
	u16 w;
	u16 h;
	HMENU id;
	LPCWSTR text;
};


/// <summary>
/// Handles the creation, execution, event 
/// </summary>
class Window
{
public:
	/// <summary>
	/// Creates a new window using a given width,
	/// height, screen position, and title.
	/// </summary>
	/// <param name="width">The width of the window.</param>
	/// <param name="height">The height of the window.</param>
	/// <param name="x">The horizontal position of the window.</param>
	/// <param name="y">The vertical position of the window.</param>
	/// <param name="title">The window title.</param>
	Window(HWND parent, u16 width, u16 height, u16 x, u16 y, const wchar_t* title, const wchar_t* _class);

	/// <summary>
	/// Creates a new window using a given width,
	/// height, screen position, and title.
	/// </summary>
	/// <param name="width">The width of the window.</param>
	/// <param name="height">The height of the window.</param>
	/// <param name="x">The horizontal position of the window.</param>
	/// <param name="y">The vertical position of the window.</param>
	/// <param name="id">The menu ID of the control.</param>
	/// <param name="title">The window title.</param>
	/// <param name="_class">The window class name.</param>
	Window(HWND parent, HINSTANCE hInstance, u16 width, u16 height, u16 x, u16 y, HMENU id, int extraFlags, const wchar_t* title, const wchar_t* _class, WIN32_EVENT(onCreate) = WIN32_DEFAULT_EVENT_HANDLER);

	// Destructor
	//
	~Window();

	/// <summary>
	/// Retrieves the current size of the window.
	/// </summary>
	/// <returns>The size of the window.</returns>
	const __forceinline Size<u16> GetWindowSize() const
	{
		return { this->width, this->height };
	}

	/// <summary>
	/// Retrieves the current location of the window.
	/// </summary>
	/// <returns>The location of the window.</returns>
	const __forceinline Pos<u16> GetWindowLocation() const
	{
		return { this->x, this->y };
	}

	/// <summary>
	/// Resizes the window to a new width and height.
	/// </summary>
	/// <param name="width">The new window width.</param>
	/// <param name="height">The new window height.</param>
	void ResizeWindow(u16 width, u16 height);

	/// <summary>
	/// Defines a new button to be displayed.
	/// This function must be called before the window's
	/// WM_CREATE event is sent.
	/// </summary>
	/// <param name="button">Input layout structure for button to be created.</param>
	void DefineButton(BUTTONREF button);

	/// <summary>
	/// The WinAPI handle to this window.
	/// </summary>
	/// <returns>A HWND for this window.</returns>
	const __forceinline HWND GetHandle() 
	{ 
		return this->handle;
	}

	/// <summary>
	/// Makes the window visible.
	/// </summary>
	__forceinline void Show() 
	{
		ShowWindow(this->handle, SW_NORMAL);
	}

	/// <summary>
	/// Runs the window message loop.
	/// </summary>
	static __forceinline int __fastcall Run()
	{
		MSG msg;

		while (GetMessage(&msg, NULL, NULL, NULL))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return HRESULT_FROM_WIN32(GetLastError());
	}

	/// <summary>
	/// Invalidates the client rect.
	/// </summary>
	__forceinline void __fastcall Invalidate(bool erase = true)
	{
		InvalidateRect(this->handle, NULL, erase);
	}

	/// <summary>
	/// Begins a paint context on this window.
	/// </summary>
	__forceinline void BeginPaint()
	{
		::BeginPaint(this->handle, &this->ps);
	}
	
	/// <summary>
	/// Ends a paint context on this window.
	/// </summary>
	__forceinline void EndPaint()
	{
		::EndPaint(this->handle, &this->ps);
	}

	/// <summary>
	/// Creates a new class definition.
	/// </summary>
	static bool __fastcall CreateClass(const wchar_t* className, COLORREF background, WNDPROC wndProc = Window::WndProcHandler);

	/// <summary>
	/// The parent window handle.
	/// </summary>
	const HWND parent;

	/** WndProc even handlers... */

	/// <summary>
	/// OnCreate event (WM_CREATE).
	/// </summary>
	WIN32_EVENT(onCreate) = WIN32_DEFAULT_EVENT_HANDLER;

	/// <summary>
	/// OnPaint event (WM_PAINT).
	/// </summary>
	WIN32_EVENT(onPaint) = WIN32_DEFAULT_EVENT_HANDLER;


private:
	/// <summary>
	/// Static window callback dispatcher.
	/// </summary>
	static LRESULT CALLBACK WndProcHandler(WNDPROC_ARGS);

	/// <summary>
	/// Member function callback handler.
	/// A `this` pointer is present as the first hidden
	/// argument to this function.
	/// </summary>
	LRESULT CALLBACK WndProc(WNDPROC_ARGS);

	u16 width;
	u16 height;

	u16 x;
	u16 y;

	HWND handle;
	PAINTSTRUCT ps;

};