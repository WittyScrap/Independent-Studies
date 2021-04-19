// Solution space function
float fnSolutionSpace (float2 v, float2 size)
{
	v = ((v / size) * 2.f - 1.f) * 3.f;

	return cos(v.x * v.y) + sin(v.x * v.y);
}