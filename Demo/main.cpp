#include <ctime>
#include <process.h>
#include <thread>
#include "Window.h"
#include "ParticleRenderer.h"
#include "Random.h"

#define INPUT_SIZEOF 24Ull

#define C_WIDTH 1270
#define C_HEIGHT 720

#define SIDEBAR_W 250
#define TITLEBAR_H 20	// HACK HACK THIS IS BAD THIS IS REALLY BAD

#define W_WIDTH (C_WIDTH + SIDEBAR_W)
#define W_HEIGHT (C_HEIGHT)

#define PARTICLE_COUNT 8192
#define PARTICLE_COUNT_2 (PARTICLE_COUNT * 2)

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

#define INIT_VELOCITY_PRECISION 1000
#define INIT_VELOCITY_RANGE 10

#define C1_START 2.f
#define C2_START 2.f
#define W_START 0.9f
#define W_MIN 0.04f

#define RNG_VELOCITY ((((float)RNG_RANGE(-INIT_VELOCITY_PRECISION, INIT_VELOCITY_PRECISION)) / (float)INIT_VELOCITY_PRECISION) * (float)INIT_VELOCITY_RANGE)

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

/// <summary>
/// Represents a single particle.
/// </summary>
struct Particle
{
	float2 position;
	float2 velocity;
	float1 sign;
	float1 __padding0;	// Padding to match size of vec2
};


// PSO vars
float C1 = C1_START;
float C2 = C2_START;
float W = W_START;
constexpr float decay = 0.001f;

// Simulation toggle
bool simulating;

/// <summary>
/// Updates the set of particles using the loaded ruleset.
/// </summary>
/// <param name="particles">The particles set.</param>
void UpdateParticles(Particle particles[PARTICLE_COUNT_2])
{
	for (int i = 0; i < PARTICLE_COUNT_2; i += 2)
	{
		particles[i].velocity.x *= (((particles[i].position.x < 0) | (particles[i].position.x > C_WIDTH )) * -1) | 1;
		particles[i].velocity.y *= (((particles[i].position.y < 0) | (particles[i].position.y > C_HEIGHT)) * -1) | 1;

		particles[i].position.x += particles[i].velocity.x * W;
		particles[i].position.y += particles[i].velocity.y * W;

		particles[i].sign.r = (particles[i].velocity.x < 0) && (particles[i].velocity.y < 0);

		particles[i + 1].position.x = particles[i].position.x + particles[i].velocity.x;
		particles[i + 1].position.y = particles[i].position.y + particles[i].velocity.y;

		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].sign.r = particles[i].sign.r;
	}

	if (W > W_MIN)
	{
		W -= decay;
	}
}

/// <summary>
/// Initializes the particle set to a random value for each particle.
/// </summary>
/// <param name="particles">The particles set.</param>
void InitParticles(Particle particles[PARTICLE_COUNT_2])
{
	for (int i = 0; i < PARTICLE_COUNT_2; i += 2)
	{
		particles[i].position.x = (float)RNG_TO(C_WIDTH);
		particles[i].position.y = (float)RNG_TO(C_HEIGHT);

		particles[i].velocity.x = RNG_VELOCITY;
		particles[i].velocity.y = RNG_VELOCITY;

		particles[i].sign.r = (particles[i].velocity.x < 0) && (particles[i].velocity.y < 0);

		particles[i + 1].position.x = particles[i].position.x + particles[i].velocity.x;
		particles[i + 1].position.y = particles[i].position.y + particles[i].velocity.y;

		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].sign.r = particles[i].sign.r;
	}
}

// The rendering unit
ParticleRenderer<Vec2>* renderer;
HWND wDisplay;
Window* canvas;
Vec2* buff;


/// <summary>
/// Paint event handler.
/// </summary>
LRESULT Update(WNDPROC_ARGS)
{
	// Grab window data
	Window* app = GRAB_WINDOW();

	renderer->RenderAndPresent();
	UpdateParticles(reinterpret_cast<Particle*>(buff));
	renderer->UpdateVectorField(buff);

	wchar_t text[10];
	swprintf(text, 10, L"W: %f", W);

	SetWindowText(wDisplay, text);

	if (simulating)
	{
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
		InitParticles(reinterpret_cast<Particle*>(buff));
		canvas->Invalidate();
		return 0;
	});

	app->RegisterButton(btnSml, (HMENU)2, WIN32_LAMBDA 
	{
		simulating = true;
		canvas->Invalidate();
		return 0;
	});

	wchar_t text[10];

	swprintf(text, 10, L"C1: %f", C1);
	app->RegisterLabel({ C_WIDTH + 10, 10, 100, 30, text }, TextAlignment::Left);

	swprintf(text, 10, L"C2: %f", C2);
	app->RegisterLabel({ C_WIDTH + 10, 40, 100, 30, text }, TextAlignment::Left);

	swprintf(text, 10, L"W: %f", W);
	wDisplay = app->RegisterLabel({ C_WIDTH + 10, 70, 100, 30, text }, TextAlignment::Left);

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