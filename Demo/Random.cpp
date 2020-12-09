#include "DataStructures.h"
#include "RandTable.h"
#include <ctime>
#include <cstdlib>


// Random pointer
static u8 pRand;

/// <summary>
/// Returns a random number.
/// </summary>
i32 NextInteger()
{
	return ((i32)rand() >> 15) | ((i32)rand());
}

/// <summary>
/// Returns a random float between a minimum and a maximum.
/// </summary>
float Range(float min, float max)
{
	return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

/// <summary>
/// Seeds the random engine.
/// </summary>
void Seed(u64 seedValue)
{
	srand((unsigned)seedValue);
}