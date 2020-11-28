#pragma once
#include <Windows.h>
#include "DataStructures.h"


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
	Window(u16 width, u16 height, u16 x, u16 y, const wchar_t* title);

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
	/// Polls the next message from this window, returns true
	/// if a message was received, false otherwise - usually,
	/// this means that the window was destroyed.
	/// </summary>
	/// <param name="oMsg">The contents of the polled message.</param>
	/// <returns>A flag that indicates whether or not a message was polled.</returns>
	bool PollMessage(UINT* oMsg);

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
	/// Resends a paint event.
	/// </summary>
	__forceinline void ForceRepaint()
	{
		InvalidateRect(this->handle, NULL, false);
	}

private:
	/// <summary>
	/// Static window callback dispatcher.
	/// </summary>
	static LRESULT CALLBACK WndProcHandler(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	u16 width;
	u16 height;

	u16 x;
	u16 y;

	HWND handle;

};