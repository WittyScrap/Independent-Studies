#ifdef __cplusplus
#ifndef D3D11_FRAMEWORK_INCLUDED
struct float2
{
	float x, y;
};
#endif
#endif

/// <summary>
/// Represents a multiple-decision solution
/// space.
/// </summary>
struct SolutionSpace
{
	float2 origin;
	float2 optionA;
	float2 optionB;
};

// Tests

#define TEST1 {{ .5f, .75f }, { .25f, .5f  }, { .75f, .5f  }}
#define TEST2 {{ .25f, .5f }, { .15f, .25f }, { .5f, .2f   }}
#define TEST3 {{ .75f, .5f }, { .5f, .15f  }, { .25f, .45f }}

#define ACTIVE TEST1

// Current solution space selection

#ifdef __cplusplus
const SolutionSpace slSpace = ACTIVE;
#else
static const SolutionSpace slSpace = ACTIVE;
#endif