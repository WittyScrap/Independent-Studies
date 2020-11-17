#pragma once
#include <Windows.h>
#include "DataStructures.h"


// Handles creating, running, and deleting
// windows.
class Window
{
public:
	// Creates a window using a specific width, height,
	// location, and title.
	//
	Window(u16 width, u16 height, u16 x, u16 y, const wchar_t* title);

	// Destructor
	//
	~Window();

	// Retrieves the current size of this window.
	//
	const __forceinline Size<u16> GetWindowSize() const
	{
		return { this->width, this->height };
	}

	// Retrieves the current location of this window.
	//
	const __forceinline Pos<u16> GetWindowLocation() const
	{
		return { this->x, this->y };
	}

	// Resizes the window to a specific size.
	//
	void ResizeWindow(u16 width, u16 height);

	// Polls the window for a message and returns it to be
	// handled.
	//
	bool PollMessage(UINT* oMsg);

	// The generated window's handle.
	//
	const __forceinline HWND GetHandle() 
	{ 
		return this->handle;
	}

	// Displays the window.
	//
	__forceinline void Show() 
	{
		ShowWindow(this->handle, SW_NORMAL);
	}

private:
	// WinProc callback dispatcher function.
	//
	static LRESULT CALLBACK WndProcHandler(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	u16 width;
	u16 height;

	u16 x;
	u16 y;

	HWND handle;
	UINT lastMsg;

};