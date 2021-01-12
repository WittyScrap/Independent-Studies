#include <ctime>
#include <process.h>
#include <thread>
#include "Window.h"
#include "Random.h"
#include "Utilities.h"
#include "PSO.h"

/** Displays are handles to controls */
typedef HWND Display;

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
#define DECL_BUFF(name) name = (Vec2*)_malloca(sizeof(Vec2) * PARTICLE_COUNT_2);
#define FREE_BUFF(name)
#endif

#define BEGIN_REGISTRATION_SEQUENCE() { u64 text;
#define REGISTER_LABEL(x, y, w, h, align, format, ...)	
#define END_REGISTRATION_SEQUENCE() }

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
Display dspWeight;
Display dspIteration;
Display dspGlobalBest;
Display dspBestX;
Display dspBestY;
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

	u64 text;

	swprintf((wchar_t*)&text, 4, L"%f", W);
	SetWindowText(dspWeight, (wchar_t*)&text);

	swprintf((wchar_t*)&text, 4, L"%f", fnSolutionSpace(globalBest));
	SetWindowText(dspGlobalBest, (wchar_t*)&text);

	swprintf((wchar_t*)&text, 4, L"%f", globalBest.x);
	SetWindowText(dspBestX, (wchar_t*)&text);

	swprintf((wchar_t*)&text, 4, L"%f", globalBest.y);
	SetWindowText(dspBestY, (wchar_t*)&text);

	swprintf((wchar_t*)&text, 4, L"%d", simulating);
	SetWindowText(dspIteration, (wchar_t*)&text);

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

	Button btnRnd = { C_WIDTH + 10, C_HEIGHT - 60, 100, 30, TEXT("Randomize") };
	Button btnSml = { C_WIDTH + 120, C_HEIGHT - 60, 100, 30, TEXT("Simulate") };

	app->RegisterButton(btnRnd, WIN32_LAMBDA 
	{
		simulating = false;
		W = W_START;
		globalBest = float2();
		InitParticles(reinterpret_cast<Particle*>(buff));
		canvas->Invalidate();
		return 0;
	});

	app->RegisterButton(btnSml, WIN32_LAMBDA 
	{
		simulating = iterations;
		canvas->Invalidate();
		return 0;
	});

	app->RegisterLabel({ C_WIDTH + 10, 10, 100, 20, TEXT("C1:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 30, 100, 20, TEXT("C2:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 50, 100, 20, TEXT("W:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 80, 100, 20, TEXT("Global Best:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 100, 100, 20, TEXT("At X:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 120, 100, 20, TEXT("At Y:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 200, 100, 20, TEXT("Iterations:") }, TextAlignment::Left);
	app->RegisterLabel({ C_WIDTH + 10, 220, 100, 20, TEXT("Step:") }, TextAlignment::Left);

	u64 text;

	swprintf((wchar_t*)&text, 4, L"%f", W);
	dspWeight = app->RegisterLabel({ C_WIDTH + 110, 50, 100, 30, (wchar_t*)&text }, TextAlignment::Left);

	swprintf((wchar_t*)&text, 4, L"%f", fnSolutionSpace(globalBest));
	dspGlobalBest = app->RegisterLabel({ C_WIDTH + 110, 80, 200, 30, (wchar_t*)&text }, TextAlignment::Left);

	swprintf((wchar_t*)&text, 4, L"%f", globalBest.x);
	xDisplay = app->RegisterLabel({ C_WIDTH + 110, 100, 200, 30, (wchar_t*)&text }, TextAlignment::Left);

	swprintf((wchar_t*)&text, 4, L"%f", globalBest.y);
	yDisplay = app->RegisterLabel({ C_WIDTH + 110, 120, 200, 30, (wchar_t*)&text }, TextAlignment::Left);

	swprintf((wchar_t*)&text, 4, L"%d", simulating);
	iDisplay = app->RegisterLabel({ C_WIDTH + 110, 220, 100, 30, (wchar_t*)&text }, TextAlignment::Left);

	wchar_t wtext[8];

	swprintf(wtext, 8, L"/ %d", iterations);
	app->RegisterLabel({ C_WIDTH + 137, 202, 100, 20, wtext }, TextAlignment::Left);

	swprintf(wtext, 8, L"%1.2f", C1);
	app->RegisterTextBox({ C_WIDTH + 110, 10, 100, 18, wtext, TextBox::Type::Any }, WIN32_LAMBDA
	{
		wchar_t text[10];
		GetWindowText(hwnd, text, 10);
		C1 = (float)_wtof(text);

		return 0;
	});

	swprintf(wtext, 8, L"%1.2f", C2); 
	app->RegisterTextBox({ C_WIDTH + 110, 30, 100, 18, wtext, TextBox::Type::Any }, WIN32_LAMBDA
	{
		wchar_t text[10];
		GetWindowText(hwnd, text, 10);
		C2 = (float)_wtof(text);

		return 0;
	});

	swprintf(wtext, 8, L"%d", iterations);
	app->RegisterTextBox({ C_WIDTH + 110, 200, 23, 18, wtext, TextBox::Type::Numeric }, WIN32_LAMBDA
	{
		wchar_t text[10];
		GetWindowText(hwnd, text, 10);
		iterations = _wtoi(text);
		/*iterations = iterations * (iterations <= ITERATIONS) + 500 * (iterations > ITERATIONS);
		_itow_s(iterations, text, 10);
		SetWindowText(hwnd, text);*/

		return 0;
	});

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