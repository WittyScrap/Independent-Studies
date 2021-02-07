#pragma once
#include "ParticleRenderer.h"
#include <math.h>

#define PARTICLE_COUNT 100
#define VECTORS_COUNT (PARTICLE_COUNT * 2)

#define INIT_VELOCITY_PRECISION RAND_MAX
#define INIT_VELOCITY_RANGE 1

#define C1_START 0.005f
#define C2_START 0.02f
#define W_START 1.f

#define W_MIN 0.50001f

#define C_WIDTH 720
#define C_HEIGHT 720

#define P_LEN 20
#define ITERATIONS 500

// The percentage of encounters in which a communication
// will be transmitted incorrectly.
#define ERROR_RATE .25f
#define ERR_RATE_NRM (ERROR_RATE * 2)

#define COMMS_DISTANCE 100

#define RNG_VELOCITY ((((float)RNG_RANGE(-INIT_VELOCITY_PRECISION, INIT_VELOCITY_PRECISION)) / (float)INIT_VELOCITY_PRECISION) * (float)INIT_VELOCITY_RANGE)
#define R Range(0, 2)

#define PROBLEM_COMPARISON <=

#include "SolutionSpaces.h"
#include "BeesSelector.h"

/// <summary>
/// Represents a single bee.
/// </summary>
struct Bee
{
	float2 position;
	float2 velocity;
	float2 localBest;
	float2 globalBest;
	float  bestValue;
};


// Bees-related vars
float C1 = C1_START;
float C2 = C2_START;
float W = W_START;
int iterations = ITERATIONS;
constexpr float decay = (1 - W_MIN) / ITERATIONS;


/// <summary>
/// Calculates the velocity for the next step.
/// </summary>
__forceinline float2 NextVelocity(Bee in)
{
	return in.velocity * W + (in.localBest - in.position) * C1 * R + (in.globalBest - in.position) * C2 * R;
}

// Simulation toggle
int step;

/// <summary>
/// Updates the set of particles using the loaded ruleset.
/// </summary>
/// <param name="particles">The particles set.</param>
void UpdateParticles(Bee particles[VECTORS_COUNT])
{
	for (int i = 0; i < VECTORS_COUNT; i += 2)
	{
		particles[i].velocity = NextVelocity(particles[i]);
		particles[i].position = particles[i].position + particles[i].velocity;

		float prevValue = particles[i].bestValue;
		float currValue = ACTIVE(particles[i].position);

		if (currValue PROBLEM_COMPARISON prevValue)
		{
			particles[i].localBest = particles[i].position;
			particles[i].globalBest = particles[i].position;
			particles[i].bestValue = currValue;
		}

		for (int p = 0; p < VECTORS_COUNT; p += 2)
		{
			int inRange = length(particles[i].position - particles[p].position) < COMMS_DISTANCE;
			int better = particles[p].bestValue PROBLEM_COMPARISON particles[i].bestValue;
			int error = R < ERR_RATE_NRM;
			
			if (inRange && (better || error))
			{
				particles[i].globalBest = particles[p].globalBest;
				particles[i].bestValue = particles[p].bestValue;
			}
		}

		particles[i + 1].position = particles[i].position + normalize(particles[i].velocity) * P_LEN;
		particles[i + 1].velocity = particles[i].velocity;
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
void InitParticles(Bee particles[VECTORS_COUNT])
{
	for (int i = 0; i < VECTORS_COUNT; i += 2)
	{
		particles[i].position.x = (float)RNG_TO(C_WIDTH);
		particles[i].position.y = (float)RNG_TO(C_HEIGHT);

		particles[i].velocity.x = RNG_VELOCITY;
		particles[i].velocity.y = RNG_VELOCITY;

		particles[i].localBest = particles[i].position;
		particles[i].globalBest = particles[i].position;
		particles[i].bestValue = ACTIVE(particles[i].position);

		particles[i + 1].position = particles[i].position + normalize(particles[i].velocity) * P_LEN;
		particles[i + 1].velocity = particles[i].velocity;
	}
}