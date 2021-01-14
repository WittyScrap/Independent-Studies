#pragma once
#include "ParticleRenderer.h"
#include <math.h>

#define PARTICLE_COUNT 1000
#define PARTICLE_COUNT_2 (PARTICLE_COUNT * 2)

#define INIT_VELOCITY_PRECISION RAND_MAX
#define INIT_VELOCITY_RANGE 1

#define C1_START 0.1f
#define C2_START 0.1f
#define W_START 1.f

#define W_MIN 0.50001f

#define C_WIDTH 720
#define C_HEIGHT 720

#define P_LEN 10
#define ITERATIONS 500

#define SPAWN_RADIUS 40

#define RNG_VELOCITY ((((float)RNG_RANGE(-INIT_VELOCITY_PRECISION, INIT_VELOCITY_PRECISION)) / (float)INIT_VELOCITY_PRECISION) * (float)INIT_VELOCITY_RANGE)
#define R Range(0, 2)

#define PROBLEM_COMPARISON <

/// <summary>
/// Represents a single particle.
/// </summary>
struct Particle
{
	float2 position;
	float2 velocity;
	float2 best;
};

// PSO vars
float C1 = C1_START;
float C2 = C2_START;
float W = W_START;
int iterations = ITERATIONS;
constexpr float decay = (1 - W_MIN) / ITERATIONS;

#include "BeesSelector.h"

/// <summary>
/// Calculates the velocity for the next step.
/// </summary>
__forceinline float2 NextVelocity(float2 inVel, float2 inPos, float2 inBest)
{
	float2 optionA = slSpace.optionA;
	float2 optionB = slSpace.optionB;

	optionA = optionA * C_WIDTH;
	optionB = optionB * C_WIDTH;

	return inVel * W + normalize(optionA - inPos) * C1 * R * inBest.x + normalize(optionB - inPos) * C1 * R * inBest.y;
}

// Simulation toggle
int step;

/// <summary>
/// Updates the set of particles using the loaded ruleset.
/// </summary>
/// <param name="particles">The particles set.</param>
void UpdateParticles(Particle particles[PARTICLE_COUNT_2])
{
	for (int i = 0; i < PARTICLE_COUNT_2; i += 2)
	{
		particles[i].velocity = NextVelocity(particles[i].velocity, particles[i].position, particles[i].best);
		particles[i].position = particles[i].position + particles[i].velocity;

		particles[i + 1].position = particles[i].position + normalize(particles[i].velocity) * P_LEN;
		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].best = particles[i].best;
	}

	if (W > W_MIN)
	{
		W -= decay;
	}

	step -= 1;
}

/// <summary>
/// Initializes the particle set to a random value for each particle.
/// </summary>
/// <param name="particles">The particles set.</param>
void InitParticles(Particle particles[PARTICLE_COUNT_2])
{
	float2 origin = slSpace.origin;
	origin = origin * C_WIDTH;

	for (int i = 0; i < PARTICLE_COUNT_2; i += 2)
	{
		float r = SPAWN_RADIUS * sqrt(Range(0, 1));
		float t = Range(0, 1) * 2 * XM_PI;

		particles[i].position.x = (float)origin.x + r * cos(t);
		particles[i].position.y = (float)origin.y + r * sin(t);

		particles[i].velocity.x = RNG_VELOCITY;
		particles[i].velocity.y = RNG_VELOCITY;

		particles[i].best = { R, R };

		particles[i + 1].position = particles[i].position + normalize(particles[i].velocity) * P_LEN;
		particles[i + 1].velocity = particles[i].velocity;
		particles[i + 1].best = particles[i].best;
	}
}