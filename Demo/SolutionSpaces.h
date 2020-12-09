#pragma once

/// <summary>
/// Test 1
/// </summary>
float fnTest1(const float2& v)
{
	float x = v.x;
	float y = v.y;

	x = ((x / C_WIDTH) * 2 - 1) * 10;
	y = ((y / C_HEIGHT) * 2 - 1) * 10;

	return (x * x) - 10 * cosf(XM_2PI * x) + (y * y) - 10 * cosf(XM_2PI * y);
}

/// <summary>
/// Test 2
/// </summary>
float fnTest2(const float2& v)
{
	float x = v.x;
	float y = v.y;

	x = ((x / C_WIDTH) * 2 - 1) * 2;
	y = ((y / C_HEIGHT) * 2 - 1) * 2;
	
	return 1 / (x * x - cosf(XM_2PI * x) + y * y - cosf(XM_2PI * y));
}

/// <summary>
/// Test 3
/// </summary>
float fnTest3(const float2& v)
{
	float x = v.x;
	float y = v.y;

	x = ((x / C_WIDTH) * 2 - 1) * 3;
	y = ((y / C_HEIGHT) * 2 - 1) * 3;

	return cosf(x * y) + sinf(x * y);
}