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
	Size<u16> GetWindowSize() const;

	// Resizes the window to a specific size.
	//
	void ResizeWindow(u16 width, u16 height);

	// Makes sure that this window was created succesfully.
	//
	const __forceinline bool IsValid() 
	{ 
		return this->hWnd;
	}

	// Displays the window.
	//
	__forceinline void Show() 
	{ 
		ShowWindow(this->hWnd, false);
	}

private:
	// WinProc callback dispatcher function.
	//
	static LRESULT CALLBACK WndProcHandler(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	// WinProc callback function.
	//
	LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	u16 width;
	u16 height;

	u16 x;
	u16 y;

	HWND hWnd;

};