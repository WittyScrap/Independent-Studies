using System.Collections;
using UnityEngine;

/// <summary>
/// Provides extension utilities for <see cref="Vector2"/> and related
/// typed objects.
/// </summary>
public static class VectorExtensions
{
	/// <summary>
	/// Assembles the <paramref name="xy"/> <see cref="Vector2"/> with an additional
	/// <see cref="float"/> parameter <paramref name="z"/> to create a <see cref="Vector3"/>,
	/// and returns it.
	/// </summary>
	public static Vector3 Assemble(Vector2 xy, float z)
	{
		return new Vector3(xy.x, xy.y, z);
	}

	/// <summary>
	/// Returns a random <see cref="Vector2"/> of exactly length <c>1</c>.
	/// </summary>
	public static Vector2 RandomUnitCircumference()
	{
		float angle = Mathf.PI * 2 * Random.value;
		return new Vector2(Mathf.Cos(angle), Mathf.Sin(angle));
	}
}