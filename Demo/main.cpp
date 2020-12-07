#include <ctime>
#include <process.h>
#include <thread>
#include "Window.h"
#include "ParticleRenderer.h"
#include "Random.h"

using thread = std::thread;

#define INPUT_SIZEOF 24Ull

#define C_WIDTH 1270
#define C_HEIGHT 720

#define SIDEBAR_W 250

#define W_WIDTH (C_WIDTH + SIDEBAR_W)
#define W_HEIGHT (C_HEIGHT)

#define PARTICLE_COUNT 8192
#define PARTICLE_COUNT_2 (PARTICLE_COUNT * 2)

#ifndef INPUT_SIZEOF
#error Please define INPUT_SIZEOF to the value of the input vertex layout structure
#endif

#if (PARTICLE_COUNT_2 * INPUT_SIZEOF) > 16384
#define DECL_BUFF(name) name = (Vec2*)malloc(sizeof(Vec2) * PARTICLE_COUNT_2);
#else
#define DECL_BUFF(name) name = (Vec2*)alloca(sizeof(Vec2) * PARTICLE_COUNT_2);
#endif

#define INIT_VELOCITY_PRECISION 1000
#define INIT_VELOCITY_RANGE 5

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

		particles[i].position.x += particles[i].velocity.x;
		particles[i].position.y += particles[i].velocity.y;

		particles[i].sign.r = (particles[i].velocity.x < 0) && (particles[i].velocity.y < 0);

		particles[i + 1].position.x = particles[i].position.x + particles[i].velocity.x;
		particles[i + 1].position.y = particles[i].position.y + particles[i].velocity.y;

		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].sign.r = particles[i].sign.r;
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
Window* canvas;
Vec2* buff;

/// <summary>
/// Paint event handler.
/// </summary>
LRESULT Update(WNDPROC_ARGS);


/// <summary>
/// Initialization event.
/// </summary>
LRESULT Initialize(WNDPROC_ARGS)
{
	// Grab window data
	ConstantBuffer cbuff = { { (float)C_WIDTH, (float)C_HEIGHT, 0, 0 }, { 1, 1, 1, 1 } };
	canvas = new Window(hwnd, GET_HINSTANCE(), C_WIDTH, C_HEIGHT, 0, 0, 0, 0, TEXT("PSO - Demo"), TEXT("WindowDX"));
	canvas->Show();
	canvas->onPaint = Update;

	renderer = new ParticleRenderer<Vec2>(canvas->GetHandle(), canvas->GetWindowSize());
	renderer->LoadShader(L"default.hlsl");
	renderer->CreateConstantBuffer<ConstantBuffer>(cbuff);
	renderer->SetVectorField(buff, PARTICLE_COUNT_2);

	BUTTONREF btnRnd = { C_WIDTH + 10, C_HEIGHT - 80, 100, 30, (HMENU)1, TEXT("Randomize") };
	BUTTONREF btnSml = { C_WIDTH + 120, C_HEIGHT - 80, 100, 30, (HMENU)2, TEXT("Simulate") };

	REGISTER_BUTTON(hwnd, GET_HINSTANCE(), btnRnd);
	REGISTER_BUTTON(hwnd, GET_HINSTANCE(), btnSml);

	return 0;
}

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

	app->Invalidate(false);

	return 0;
}

// Main entry point for demo
//
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	Window::CreateClass(L"WindowMainPanel", COLOR_WINDOW);
	Window::CreateClass(L"WindowDX", RGB(255, 255, 255));

	Seed((u8)((u64)time(NULL) * (u32)_getpid()));

	DECL_BUFF(buff);
	InitParticles(reinterpret_cast<Particle*>(buff));

	Window main(NULL, GetModuleHandle(NULL), W_WIDTH, W_HEIGHT, CW_USEDEFAULT, CW_USEDEFAULT, (HMENU)0, 0, TEXT("PSO - Demo"), TEXT("WindowMainPanel"), Initialize);
	main.Show();

	Window::Run();
}