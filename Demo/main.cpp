#include "Window.h"

// Main entry point for demo
//
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	Window canvas(1270, 720, 100, 100, L"PSO - Demonstration");
	canvas.Show();

	UINT msg;

	while (canvas.PollMessage(&msg))
	{
		switch (msg)
		{
		// Test
		case WM_PAINT:
			HDC dc = GetDC(canvas.GetHandle());
			MoveToEx(dc, 10, 10, NULL);
			LineTo(dc, 100, 100);
			ReleaseDC(canvas.GetHandle(), dc);
			break;

		}
	}
}