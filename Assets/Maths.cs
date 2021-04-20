using UnityEngine;

/// <summary>
/// Mathematics extension methods.
/// </summary>
public static class Maths
{
    /// <summary>
    /// Inflates a <see cref="Rect"/> by a given <paramref name="amount"/>.
    /// </summary>
    public static Rect Inflated(this Rect rect, float amount)
    {
        return new Rect
        (
            rect.x - amount,
            rect.y - amount,
            rect.width + amount,
            rect.height + amount
        );
    }
}