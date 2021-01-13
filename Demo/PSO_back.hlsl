#include "background_vert.slinc"
#include "PSO_solutions.slinc"

#define TEST1(tex) fnTest1(tex) / 200
#define TEST2(tex) fnTest2(tex)
#define TEST3(tex) fnTest3(tex)

#define FN(tex) TEST1((tex * 2 - 1))


/**
 *  Pixel function/shader. This function must match
 *  the signature given in the compilation phase.
 *
 */
float4 pixel(vout i) : SV_Target
{
	const float4 lowEnd = float4(0.25f, 0.125f, 0, 1);
	const float4 highEnd = float4(0.9f, 0.5f, 0, 1);

	return lerp(lowEnd, highEnd, saturate(FN(i.texcoord)));
}