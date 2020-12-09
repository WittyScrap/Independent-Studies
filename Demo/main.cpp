#include <ctime>
#include <process.h>
#include <thread>
#include "Window.h"
#include "Random.h"
#include "PSO.h"

#define INPUT_SIZEOF 24Ull

#define SIDEBAR_W 250
#define TITLEBAR_H 20	// HACK HACK THIS IS BAD THIS IS REALLY BAD

#define W_WIDTH (C_WIDTH + SIDEBAR_W)
#define W_HEIGHT (C_HEIGHT)

#ifndef INPUT_SIZEOF
#error Please define INPUT_SIZEOF to the value of the input vertex layout structure
#endif

#if (PARTICLE_COUNT_2 * INPUT_SIZEOF) > 16384
#define DECL_BUFF(name) name = (Vec2*)malloc(sizeof(Vec2) * PARTICLE_COUNT_2);
#define FREE_BUFF(name) free(name)
#else
#define DECL_BUFF(name) name = (Vec2*)alloca(sizeof(Vec2) * PARTICLE_COUNT_2);
#define FREE_BUFF(name)
#endif

#define ABS(x) (x) * (((x) > 0) - ((x) < 0))

/// <summary>
/// Constant buffer definition for default shader.
/// </summary>
__declspec(align(16))
struct ConstantBuffer
{
	float4 scale;		// float2 for value + float2 for padding
	float4 color;
};

// The rendering unit
ParticleRenderer<Vec2>* renderer;
HWND wDisplay;
HWND iDisplay;
HWND bDisplay;
HWND xDisplay;
HWND yDisplay;
Window* canvas;
Vec2* buff;


/// <summary>
/// Paint event handler.
/// </summary>
LRESULT Update(WNDPROC_ARGS)
{
	// Grab window data
	Window* app = GRAB_WINDOW();

	renderer->UpdateVectorField(buff);
	renderer->RenderAndPresent();
	wchar_t text[20];

	swprintf(text, 10, L"W: %f", W);
	SetWindowText(wDisplay, text);

	swprintf(text, 20, L"Iteration: %d", simulating);
	SetWindowText(iDisplay, text);

	swprintf(text, 20, L"G-Best: %f", fnSolutionSpace(globalBest));
	SetWindowText(bDisplay, text);

	swprintf(text, 20, L"At X: %f", globalBest.x);
	SetWindowText(xDisplay, text);

	swprintf(text, 20, L"At Y: %f", globalBest.y);
	SetWindowText(yDisplay, text);

	if (simulating)
	{
		UpdateParticles(reinterpret_cast<Particle*>(buff));
		app->Invalidate(false);
	}

	return 0;
}

/// <summary>
/// Initialization event.
/// </summary>
LRESULT Initialize(WNDPROC_ARGS)
{
	Window* app = GRAB_WINDOW();

	ConstantBuffer cbuff = { { (float)C_WIDTH, (float)C_HEIGHT, 0, 0 }, { 1, 1, 1, 1 } };
	canvas = new Window(hwnd, GET_HINSTANCE(), C_WIDTH, C_HEIGHT, 0, 0, 0, 0, TEXT("PSO - Demo"), TEXT("WindowDX"));
	canvas->Show();
	canvas->onPaint = Update;

	renderer = new ParticleRenderer<Vec2>(canvas->GetHandle(), canvas->GetWindowSize());
	renderer->LoadShader(L"default.hlsl");
	renderer->CreateConstantBuffer<ConstantBuffer>(cbuff);
	renderer->SetVectorField(buff, PARTICLE_COUNT_2);

	BUTTONREF btnRnd = { C_WIDTH + 10, C_HEIGHT - 60, 100, 30, TEXT("Randomize") };
	BUTTONREF btnSml = { C_WIDTH + 120, C_HEIGHT - 60, 100, 30, TEXT("Simulate") };


	app->RegisterButton(btnRnd, (HMENU)1, WIN32_LAMBDA 
	{
		simulating = false;
		W = W_START;
		globalBest = float2();
		InitParticles(reinterpret_cast<Particle*>(buff));
		canvas->Invalidate();
		return 0;
	});

	app->RegisterButton(btnSml, (HMENU)2, WIN32_LAMBDA 
	{
		simulating = ITERATIONS;
		canvas->Invalidate();
		return 0;
	});

	wchar_t text[20];

	swprintf(text, 10, L"C1: %f", C1);
	app->RegisterLabel({ C_WIDTH + 10, 10, 100, 30, text }, TextAlignment::Left);

	swprintf(text, 10, L"C2: %f", C2);
	app->RegisterLabel({ C_WIDTH + 10, 40, 100, 30, text }, TextAlignment::Left);

	swprintf(text, 10, L"W: %f", W);
	wDisplay = app->RegisterLabel({ C_WIDTH + 10, 70, 100, 30, text }, TextAlignment::Left);

	swprintf(text, 20, L"Iteration: %d", simulating);
	iDisplay = app->RegisterLabel({ C_WIDTH + 10, 110, 100, 30, text }, TextAlignment::Left);

	swprintf(text, 20, L"G-Best: %f", fnSolutionSpace(globalBest));
	bDisplay = app->RegisterLabel({ C_WIDTH + 10, 140, 200, 30, text }, TextAlignment::Left);

	swprintf(text, 20, L"At X: %f", globalBest.x);
	xDisplay = app->RegisterLabel({ C_WIDTH + 10, 170, 200, 30, text }, TextAlignment::Left);

	swprintf(text, 20, L"At Y: %f", globalBest.y);
	yDisplay = app->RegisterLabel({ C_WIDTH + 10, 200, 200, 30, text }, TextAlignment::Left);

	return 0;
}

// Main entry point for demo
//
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	Window::CreateClass(L"WindowMainPanel", (HBRUSH)(COLOR_WINDOW));
	Window::CreateClass(L"WindowDX", (HBRUSH)(COLOR_WINDOW));

	Seed((u8)((u64)time(NULL) * (u32)_getpid()));

	DECL_BUFF(buff);
	InitParticles(reinterpret_cast<Particle*>(buff));
	
	simulating = 0;

	Window main(NULL, GetModuleHandle(NULL), W_WIDTH, W_HEIGHT + TITLEBAR_H, (u16)CW_USEDEFAULT, (u16)CW_USEDEFAULT, (HMENU)0, 0, TEXT("PSO - Demo"), TEXT("WindowMainPanel"), Initialize);
	main.Show();

	Window::Run();

	delete renderer;
	delete canvas;
	FREE_BUFF(buff);
}