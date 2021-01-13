#include "background_vert.slinc"
#include "PSO_solutions.slinc"
#include "PSOSelector.h"

#define FN(tex) ACTIVE((tex * 2 - 1))

/**
 *  Pixel function/shader. This function must match
 *  the signature given in the compilation phase.
 *
 */
float4 pixel(vout i) : SV_Target
{
	const float4 lowEnd  = float4(0, 0, 1, 1);
	const float4 midEnd  = float4(0, 1, 0, 1);
	const float4 highEnd = float4(1, 0, 0, 1);

	float fn = saturate(FN(i.texcoord));

	float4 lowLerp = lerp(lowEnd, midEnd, saturate(fn * 2));
	float4 highLerp = lerp(midEnd, highEnd, saturate(fn / 2 + .5f));

	return lerp(lowLerp, highLerp, fn) / 2;
}