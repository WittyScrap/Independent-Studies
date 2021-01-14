#include "../Rendering/background_vert.slinc"
#include "BeesSelector.h"

#define m 0.5f
#define r .05f;

/**
 *  Pixel function/shader. This function must match
 *  the signature given in the compilation phase.
 *
 */
float4 pixel(vout i) : SV_Target
{
	const float4 origin  = { 1, m, m, 1 };
	const float4 optionA = { m, 1, m, 1 };
	const float4 optionB = { m, m, 1, 1 };

	i.texcoord.y = 1 - i.texcoord.y;

	float dstToOrigin  = length(i.texcoord - slSpace.origin);
	float dstToOptionA = length(i.texcoord - slSpace.optionA);
	float dstToOptionB = length(i.texcoord - slSpace.optionB);

	float inOrigin  = dstToOrigin  < r;
	float inOptionA = dstToOptionA < r;
	float inOptionB = dstToOptionB < r;

	float inAny = saturate(inOrigin + inOptionA + inOptionB);
	float4 tint = inOrigin * origin + inOptionA * optionA + inOptionB * optionB;

	return lerp(.5f, tint, inAny);
}