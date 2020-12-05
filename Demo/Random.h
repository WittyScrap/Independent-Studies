#pragma once
#define RNG() NextInteger()
#define RNG_RANGE(a, b) ((NextInteger() + a) % b)
#define RNG_FROM(a) (NextInteger() + a)
#define RNG_TO(b) (NextInteger() % b)

/// <summary>
/// Returns a random integer.
/// </summary>
u16 NextInteger();

/// <summary>
/// Seeds the random engine.
/// </summary>
void Seed(u8 seedValue);