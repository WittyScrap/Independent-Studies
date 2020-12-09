#pragma once
#include "ParticleRenderer.h"
#include <math.h>

#define PARTICLE_COUNT 100
#define PARTICLE_COUNT_2 (PARTICLE_COUNT * 2)

#define INIT_VELOCITY_PRECISION RAND_MAX
#define INIT_VELOCITY_RANGE 1

#define C1_START 0.01f
#define C2_START 0.01f
#define W_START 1.f

#define W_MIN 0.50001f

#define C_WIDTH 720
#define C_HEIGHT 720

#define P_LEN 5
#define ITERATIONS 500

#define RNG_VELOCITY ((((float)RNG_RANGE(-INIT_VELOCITY_PRECISION, INIT_VELOCITY_PRECISION)) / (float)INIT_VELOCITY_PRECISION) * (float)INIT_VELOCITY_RANGE)
#define R Range(0, 2)

#include "SolutionSpaces.h"

/// <summary>
/// Represents a single particle.
/// </summary>
struct Particle
{
	float2 position;
	float2 velocity;
	float2 localBest;
};


// PSO vars
float C1 = C1_START;
float C2 = C2_START;
float W = W_START;
constexpr float decay = (1 - W_MIN) / ITERATIONS;

// Current global best
float2 globalBest;

/// <summary>
/// Function for the solution space.
/// </summary>
/// <param name="x">X-coordinate</param>
/// <param name="y">Y-coordinate</param>
/// <returns>The value at the given X/Y coordinate</returns>
float (*fnSolutionSpace)(const float2& v) = fnTest1;

/// <summary>
/// Calculates the velocity for the next step.
/// </summary>
__forceinline float2 NextVelocity(float2 inVel, float2 inPos, float2 inBest)
{
	return inVel * W + (inBest - inPos) * C1 * R + (globalBest - inPos) * C2 * R;
}

// Simulation toggle
int simulating;

/// <summary>
/// Updates the set of particles using the loaded ruleset.
/// </summary>
/// <param name="particles">The particles set.</param>
void UpdateParticles(Particle particles[PARTICLE_COUNT_2])
{
	for (int i = 0; i < PARTICLE_COUNT_2; i += 2)
	{
		particles[i].velocity = NextVelocity(particles[i].velocity, particles[i].position, particles[i].localBest);
		particles[i].position = particles[i].position + particles[i].velocity;

		float prevValue = fnSolutionSpace(particles[i].localBest);
		float currValue = fnSolutionSpace(particles[i].position);
		float globalValue = fnSolutionSpace(globalBest);

		if (currValue < prevValue)
		{
			particles[i].localBest = particles[i].position;
		}

		if (currValue < globalValue)
		{
			globalBest = particles[i].position;
		}

		particles[i + 1].position = particles[i].position + normalize(particles[i].velocity) * P_LEN;
		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].localBest = particles[i].localBest;
	}

	if (W > W_MIN)
	{
		W -= decay;
	}

	simulating -= 1;
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

		particles[i].localBest = particles[i].position;

		particles[i + 1].position = particles[i].position + normalize(particles[i].velocity) * P_LEN;
		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].localBest = particles[i].localBest;
	}
}