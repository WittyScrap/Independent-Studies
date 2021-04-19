using UnityEngine.SceneManagement;
using UnityEngine;

/// <summary>
/// Handles the creation, response, and interaction of various UI elements.
/// </summary>
public class UI : MonoBehaviour
{
    /// <summary>
    /// A window that can display a graph that tracks one or more variables across a 2D
    /// space.
    /// </summary>
    public class GraphWindow 
    {
        public readonly string XAxis;

        public readonly string YAxis;

        private readonly Texture2D _graph;

        
    }

    private static readonly string RestartText = "Restart...";

    [SerializeField]
    private GUISkin _guiSkin;

    [SerializeField]
    private Rect _restartButton;
    

    public void Start()
    {
        if (_guiSkin != null)
        {
            GUI.skin = _guiSkin;
        }
    }

    public void OnGUI()
    {
        if (GUI.Button(_restartButton, RestartText))
        {
            SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        }
    }
}
