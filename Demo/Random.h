#pragma once
#define RNG() NextInteger()
#define RNG_RANGE(a, b) ((a) + RNG() / (RAND_MAX / ((b) - (a) + 1) + 1))
#define RNG_FROM(a) (RNG() + (a))
#define RNG_TO(b) (RNG() % (b))

/// <summary>
/// Returns a random integer.
/// </summary>
i32 NextInteger();

/// <summary>
/// Seeds the random engine.
/// </summary>
void Seed(u64 seedValue);