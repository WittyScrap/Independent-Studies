using System.Collections;
using UnityEngine;

/// <summary>
/// Provides extension utilities for <see cref="Texture"/> and related
/// typed objects.
/// </summary>
public static class TextureExtensions
{
	/// <summary>
	/// Returns the width and height of the given <paramref name="texture"/>
	/// bundled together into a <see cref="Vector2Int"/>.
	/// </summary>
	public static Vector2Int GetSize(this Texture2D texture)
	{
		return new Vector2Int(texture.width, texture.height);
	}

	/// <summary>
	/// Returns the width and height of the given <paramref name="texture"/>
	/// bundled together into a <see cref="Vector2Int"/>.
	/// </summary>
	public static Vector2Int GetSize(this RenderTexture texture)
	{
		return new Vector2Int(texture.width, texture.height);
	}
}