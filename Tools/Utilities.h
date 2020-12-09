#pragma once
#include <stdint.h>
#include <stdio.h>

/// <summary>
/// Converts a float to an 8-byte string.
/// </summary>
__forceinline uint64_t ToString(const float& value)
{
	uint64_t txt;
	sprintf_s((char*)&txt, 8, "%f", value);
	return txt;
}

/// <summary>
/// Converts a float to a 4-byte wstring.
/// </summary>
__forceinline uint64_t wToString(const float& value)
{
	uint64_t txt;
	swprintf((wchar_t*)&txt, 4, L"%f", value);
	return txt;
}