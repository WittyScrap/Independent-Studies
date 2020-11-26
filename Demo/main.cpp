#include "Window.h"
#include "ParticleRenderer.h"

#define W_WIDTH 1270
#define W_HEIGHT 720

/// <summary>
/// Constant buffer definition for default shader.
/// </summary>
__declspec(align(16))
struct ConstantBuffer
{
	float4 scale;
	float4 color;
};

// Main entry point for demo
//
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	Window canvas(W_WIDTH, W_HEIGHT, 100, 100, L"PSO - Demonstration");
	canvas.Show();

	ConstantBuffer cbuff = { { (float)W_WIDTH, (float)W_HEIGHT, 0, 0 }, { 1, 1, 1, 1 } };

	ParticleRenderer<> renderer(canvas.GetHandle(), canvas.GetWindowSize());
	renderer.LoadShader(L"default.hlsl");
	renderer.CreateConstantBuffer<ConstantBuffer>(cbuff);

	Vec2 buff[2];
	buff[0] = { { 0, 0 }, { 1, 1, 1, 1 } };
	buff[1] = { { 0.5, 0.5 }, { 1, 1, 1, 1 } };

	renderer.SetVectorField(buff, ARRAYSIZE(buff));

	UINT msg;

	while (canvas.PollMessage(&msg))
	{
		switch (msg)
		{
		// Test
		case WM_PAINT:
			renderer.RenderAndPresent();
			break;

		}
	}
}