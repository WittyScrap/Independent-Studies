// Solution space function
float fnSolutionSpace (const float2 v)
{
	float x = v.x;
	float y = v.y;

	x = ((x / OutputWidth) * 2 - 1) * 3;
	y = ((y / OutputHeight) * 2 - 1) * 3;

	return cos(x * y) + sin(x * y);
}