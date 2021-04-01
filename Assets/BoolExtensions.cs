using System.Collections;
using UnityEngine;

/// <summary>
/// Contains extensions for converting <c>bool</c>eans into different integral types
/// and performing other related branchless operations.
/// </summary>
public unsafe static class BoolExtensions
{
    /// <summary>
    /// Reinterpret-casts a <c>bool</c> down into a <c>byte</c> container.
    /// This operation is expected to be compiled into a single <c>mov</c> expression.
    /// </summary>
    public static byte ToByte(this bool @bool)
    {
        return *(byte*)(&@bool);
    }

    /// <summary>
    /// Reinterpret-casts a <c>bool</c> down into a <c>byte</c> container.
    /// </summary>
    public static float ToSingle(this bool @bool)
    {
        return *(byte*)(&@bool);
    }

    /// <summary>
    /// Compiles down to a branchless ternary <c>if</c> expression. If the given
    /// <paramref name="condition"/> is <c>true</c>, then the <paramref name="true"/> value
    /// will be returned. Otherwise, the <paramref name="false"/> value is returned.
    /// </summary>
    public static int If(this bool condition, in byte @true, in byte @false)
    {
        return condition.ToByte() * @true + (!condition).ToByte() * @false;
    }

    /// <summary>
    /// Compiles down to a branchless ternary <c>if</c> expression. If the given
    /// <paramref name="condition"/> is <c>true</c>, then the <paramref name="true"/> value
    /// will be returned. Otherwise, the <paramref name="false"/> value is returned.
    /// </summary>
    public static float If(this bool condition, in float @true, in float @false)
    {
        return condition.ToByte() * @true + (!condition).ToByte() * @false;
    }
}