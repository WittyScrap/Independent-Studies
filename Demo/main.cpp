#include <ctime>
#include <process.h>
#include "Window.h"
#include "ParticleRenderer.h"
#include "Random.h"

#define C_WIDTH 1270
#define C_HEIGHT 720

#define SIDEBAR_W 250

#define W_WIDTH (C_WIDTH + SIDEBAR_W)
#define W_HEIGHT (C_HEIGHT)

#define PARTICLE_COUNT_2 4096
#define INIT_VELOCITY_MAX 50
#define INIT_VELOCITY (NextInteger() % INIT_VELOCITY_MAX) - (INIT_VELOCITY_MAX / 2)

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
	float2 __padding0;	// Padding to match size of vec2
};


/// <summary>
/// Updates the set of particles using the loaded ruleset.
/// </summary>
/// <param name="particles">The particles set.</param>
void UpdateParticles(Particle particles[PARTICLE_COUNT_2])
{
	for (int i = 0; i < PARTICLE_COUNT_2; i += 2)
	{
		particles[i] = {
			{ particles[i].position.x + particles[i].velocity.x, particles[i].position.y + particles[i].velocity.y },
			particles[i].velocity
		};

		particles[i + 1] = {
			{ particles[i].position.x + particles[i].velocity.x, particles[i].position.y + particles[i].velocity.y },
			{ 0 }
		};
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
		particles[i] = {
			{ (float)RNG_TO(C_WIDTH), (float)RNG_TO(C_HEIGHT) },
			{ (float)INIT_VELOCITY,	  (float)INIT_VELOCITY	  }
		};

		particles[i + 1] = {
			{ particles[i].position.x + particles[i].velocity.x, particles[i].position.y + particles[i].velocity.y },
			{ 0 }
		};
	}
}

// Main entry point for demo
//
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	Window main(NULL, W_WIDTH, W_HEIGHT, 100, 100, L"PSO - Demonstration");
	main.Show();

	Window canvas(main.GetHandle(), C_WIDTH, C_HEIGHT, 0, 0, L"PSO - Demonstration");
	canvas.Show();

	ConstantBuffer cbuff = { { (float)C_WIDTH, (float)C_HEIGHT, 0, 0 }, { 1, 1, 1, 1 } };

	ParticleRenderer<> renderer(canvas.GetHandle(), canvas.GetWindowSize());
	renderer.LoadShader(L"default.hlsl");
	renderer.CreateConstantBuffer<ConstantBuffer>(cbuff);

	Seed((u8)((u64)time(NULL) * (u32)_getpid()));

	Vec2 buff[PARTICLE_COUNT_2];

	InitParticles(reinterpret_cast<Particle*>(buff));
	renderer.SetVectorField(buff, ARRAYSIZE(buff));

	MSG msg_main;
	MSG msg_canvas;

	while (main.PollMessage(&msg_main))
	{
		// Switch between canvas messages
		switch (msg_main.message)
		{
			// Update()
			case WM_PAINT:
			{
				UpdateParticles(reinterpret_cast<Particle*>(buff));
				renderer.UpdateVectorField(buff);
				renderer.RenderAndPresent();
				break;
			}
		}

		// Force a repaint event
		canvas.ForceRepaint();
	}
}