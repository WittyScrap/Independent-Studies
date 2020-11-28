#include "DataStructures.h"
#include "RandTable.h"
#include <ctime>

// Random pointer
static u8 pRand;

/// <summary>
/// Returns a random number.
/// </summary>
u16 NextInteger()
{
	pRand += (u8)time(NULL);
	return randTable[pRand];
}

/// <summary>
/// Seeds the random engine.
/// </summary>
void Seed(u8 seedValue)
{
	pRand = seedValue;
}