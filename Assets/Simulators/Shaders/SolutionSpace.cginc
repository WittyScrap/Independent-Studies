#define XM_2PI 6.28318530718

// "Holes" map
float fnTest1(float2 v, float2 size)
{
	v = ((v / size) * 2.f - 1.f) * 2.f;
	return 1 / (v.x * v.x - cos(XM_2PI * v.x) + v.y * v.y - cos(XM_2PI * v.y));
}

// "Valley" map
float fnTest2(float2 v, float2 size)
{
	v = ((v / size) * 2.f - 1.f) * 2.f;
	return (v.x * v.x) - 2 * cos(XM_2PI * 4 * v.x) + (v.y * v.y) - 2 * cos(XM_2PI * 4 * v.y);
}

// "Canyon" map
float fnTest3(float2 v, float2 size)
{
	v = ((v / size) * 2.f - 1.f) * 3.f;
	return cos(v.x * v.y) + sin(v.x * v.y);
}

// Solution space function
#define fnSolutionSpace(v, size) fnTest3(v, size)