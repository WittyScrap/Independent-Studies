#pragma once
#pragma warning(disable: 4003)
#include <d3d11.h>
#include <Windows.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl.h>
#include "DataStructures.h"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define D3D11_FRAMEWORK_INCLUDED

#if _DEBUG
#define D3D_FLAGS D3D11_CREATE_DEVICE_DEBUG
#else
#define D3D_FLAGS 0
#endif

#define EXPAND(...) { __VA_ARGS__ }
// More namestyle-consistent dynamic assert
#define ASSERT(expression) assert(expression)

#if _DEBUG
#define CHECK(expression) ASSERT(expression)
#define TRY(expression, retval) if (!!(expression)) return retval;
#define DEBUG_IF(expression, body) if (!!(expression)) body
#define DEBUG_DO(expression, alt) expression
#else
// This feature is only available in debug mode.
#define CHECK(nil)
// This feature is only available in debug mode.
#define TRY(expression, nil) expression
// This feature is only available in debug mode.
#define DEBUG_IF(nil, nil2)
#define DEBUG_DO(expression, alt) alt
#endif

#define D3D_DRIVERS_LIST D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
#define D3D_FEATURES_LIST D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0

#define D3D_DRIVERS EXPAND(D3D_DRIVERS_LIST)
#define D3D_FEATURES EXPAND(D3D_FEATURES_LIST)

#define D3D_ANTIALIASING_AMOUNT 1
#define D3D_ANTIALIASING_QUALITY 0

#define CBUFFER __declspec(align(16)) struct

using namespace DirectX;
using namespace Microsoft::WRL;

template<typename TCOMType>
using com = ComPtr<TCOMType>;

/// <summary>
/// 1-dimensional float
/// </summary>
struct float1
{
	union
	{
		float x;
		float r;
	};

	__forceinline float1 operator+(const float1& rhs)
	{
		return { x + rhs.x };
	}

	__forceinline float1 operator-(const float1& rhs)
	{
		return { x - rhs.x };
	}

	__forceinline float1 operator*(const float& rhs)
	{
		return { x * rhs };
	}
};

/// <summary>
/// 2-dimensional float
/// </summary>
struct float2
{
	union
	{
		struct
		{
			float x;
			float y;
		};

		struct
		{
			float r;
			float g;
		};
	};

	__forceinline float2 operator+(const float2& rhs)
	{
		return {
			x + rhs.x,
			y + rhs.y
		};
	}

	__forceinline float2 operator-(const float2& rhs)
	{
		return {
			x - rhs.x,
			y - rhs.y
		};
	}

	__forceinline float2 operator*(const float& rhs)
	{
		return {
			x * rhs,
			y * rhs
		};
	}
};

/// <summary>
/// 3-dimensional float
/// </summary>
struct float3
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		struct
		{
			float r;
			float g;
			float b;
		};
	};

	__forceinline float3 operator+(const float3& rhs)
	{
		return {
			x + rhs.x,
			y + rhs.y,
			z + rhs.z
		};
	}

	__forceinline float3 operator-(const float3& rhs)
	{
		return {
			x - rhs.x,
			y - rhs.y,
			z - rhs.z
		};
	}

	__forceinline float3 operator*(const float& rhs)
	{
		return {
			x * rhs,
			y * rhs,
			z * rhs
		};
	}
};

/// <summary>
/// 4-dimensional float
/// </summary>
struct float4
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
	};

	__forceinline float4 operator+(const float4& rhs)
	{
		return {
			x + rhs.x,
			y + rhs.y,
			z + rhs.z,
			w + rhs.w
		};
	}

	__forceinline float4 operator-(const float4& rhs)
	{
		return {
			x - rhs.x,
			y - rhs.y,
			z - rhs.z,
			w - rhs.w
		};
	}

	__forceinline float4 operator*(const float& rhs)
	{
		return {
			x * rhs,
			y * rhs,
			z * rhs,
			w * rhs
		};
	}
};

/// <summary>
/// 1-dimensional integer.
/// </summary>
struct int1 {
	INT32 x;
};

/// <summary>
/// 2-dimensional integer.
/// </summary>
struct int2 {
	INT32 x;
	INT32 y;
};

/// <summary>
/// 3-dimensional integer.
/// </summary>
struct int3 {
	INT32 x;
	INT32 y;
	INT32 z;
};

/// <summary>
/// 4-dimensional integer.
/// </summary>
struct int4 {
	INT32 x;
	INT32 y;
	INT32 z;
	INT32 w;
};

/// <summary>
/// 3x3 matrix.
/// </summary>
struct float2x2
{
	float M11; float M12;
	float M21; float M22;

	/// <summary>
	/// Linearly interpolates between the two given matrices.
	/// </summary>
	static __forceinline void Lerp(float2x2& o, float2x2 a, float2x2 b, float t)
	{
		o.M11 = a.M11 * (1 - t) + b.M11 * t;
		o.M12 = a.M12 * (1 - t) + b.M12 * t;

		o.M21 = a.M21 * (1 - t) + b.M21 * t;
		o.M22 = a.M22 * (1 - t) + b.M22 * t;
	}
};

/// <summary>
/// 3x3 matrix.
/// </summary>
struct float3x3
{
	float M11; float M12; float M13;
	float M21; float M22; float M23;
	float M31; float M32; float M33;

	/// <summary>
	/// Linearly interpolates between the two given matrices.
	/// </summary>
	static __forceinline void Lerp(float3x3& o, float3x3 a, float3x3 b, float t)
	{
		o.M11 = a.M11 * (1 - t) + b.M11 * t;
		o.M12 = a.M12 * (1 - t) + b.M12 * t;
		o.M13 = a.M13 * (1 - t) + b.M13 * t;

		o.M21 = a.M21 * (1 - t) + b.M21 * t;
		o.M22 = a.M22 * (1 - t) + b.M22 * t;
		o.M23 = a.M23 * (1 - t) + b.M23 * t;

		o.M31 = a.M31 * (1 - t) + b.M31 * t;
		o.M32 = a.M32 * (1 - t) + b.M32 * t;
		o.M33 = a.M33 * (1 - t) + b.M33 * t;
	}
};

/// <summary>
///  A 4x4 matrix container.
/// </summary>
struct float4x4
{
	float M11; float M12; float M13; float M14;
	float M21; float M22; float M23; float M24;
	float M31; float M32; float M33; float M34;
	float M41; float M42; float M43; float M44;

	/// <summary>
	/// Linearly interpolates between the two given matrices.
	/// </summary>
	static __forceinline void Lerp(float4x4& o, float4x4 a, float4x4 b, float t)
	{
		o.M11 = a.M11 * (1 - t) + b.M11 * t;
		o.M12 = a.M12 * (1 - t) + b.M12 * t;
		o.M13 = a.M13 * (1 - t) + b.M13 * t;
		o.M14 = a.M14 * (1 - t) + b.M14 * t;

		o.M21 = a.M21 * (1 - t) + b.M21 * t;
		o.M22 = a.M22 * (1 - t) + b.M22 * t;
		o.M23 = a.M23 * (1 - t) + b.M23 * t;
		o.M24 = a.M24 * (1 - t) + b.M24 * t;

		o.M31 = a.M31 * (1 - t) + b.M31 * t;
		o.M32 = a.M32 * (1 - t) + b.M32 * t;
		o.M33 = a.M33 * (1 - t) + b.M33 * t;
		o.M34 = a.M34 * (1 - t) + b.M34 * t;

		o.M41 = a.M41 * (1 - t) + b.M41 * t;
		o.M42 = a.M42 * (1 - t) + b.M42 * t;
		o.M43 = a.M43 * (1 - t) + b.M43 * t;
		o.M44 = a.M44 * (1 - t) + b.M44 * t;
	}
};

/// <summary>
/// Default input vertex layout.
/// </summary>
struct Vec2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		float2 pos;
	};

	union
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		float4 color;
	};
};

/// <summary>
/// Alternate input vertex layout.
/// </summary>
struct Tex2
{
	union
	{
		struct
		{
			float x;
			float y;
		};
		float2 pos;
	};

	union
	{
		struct
		{
			float u;
			float v;
		};
		float2 uv;
	};
};

/// <summary>
/// Finds the length of the 2 dimensional
/// vector.
/// </summary>
__forceinline float length(const float2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

/// <summary>
/// Finds the length of the 3 dimensional
/// vector.
/// </summary>
__forceinline float length(const float3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

/// <summary>
/// Finds the length of the 4 dimensional
/// vector.
/// </summary>
__forceinline float length(const float4& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

/// <summary>
/// Normalizes a 2D vector.
/// </summary>
__forceinline float2 normalize(const float2& v)
{
	float len = length(v);
	return {
		v.x / len,
		v.y / len
	};
}

/// <summary>
/// Normalizes a 3D vector.
/// </summary>
__forceinline float3 normalize(const float3& v)
{
	float len = length(v);
	return {
		v.x / len,
		v.y / len,
		v.z / len
	};
}

/// <summary>
/// Normalizes a 4D vector.
/// </summary>
__forceinline float4 normalize(const float4& v)
{
	float len = length(v);
	return {
		v.x / len,
		v.y / len,
		v.z / len,
		v.w / len
	};
}