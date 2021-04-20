using UnityEngine.SceneManagement;
using UnityEngine;
using UnityEditor;

/// <summary>
/// Handles the creation, response, and interaction of various UI elements.
/// </summary>
public class UI : MonoBehaviour
{
    /// <summary>
    /// A window that can display a graph that tracks one or more variables across a 2D
    /// space.
    /// </summary>
    public class GraphWindow : EditorWindow
    {
        /// <summary>
        /// The label to assign to the <c>X</c> axis.
        /// </summary>
        public readonly string xAxis;

        /// <summary>
        /// The label to assign to the <c>Y</c> axis.
        /// </summary>
        public readonly string yAxis;

        private const float FrameSize = 10;

        private readonly Texture2D _graph;

        private int _cursor = 0;

        
        public GraphWindow(in Rect window, in string xAxis, in string yAxis)
        {
            this.xAxis = xAxis;
            this.yAxis = yAxis;

            int width = Mathf.RoundToInt(position.width);
            int height = Mathf.RoundToInt(position.height);

            _graph = new Texture2D(width, height);
        }

        public void OnGUI()
        {
            GUI.DrawTexture(new Rect(Vector2.zero, position.size), _graph);
        }

        public void OnDestroy()
        {
            DestroyImmediate(_graph);
        }

        /// <summary>
        /// Plots a new entry of value Y at the current cursor position, without advancing it.
        /// </summary>
        public void Plot(in float atY, in Color color)
        {
            int y = Mathf.RoundToInt(atY * position.height);
            _graph.SetPixel(_cursor, y, color);
        }

        /// <summary>
        /// Advances the X axis cursor by the provided amount (default: 1).
        /// </summary>
        public void Advance(in int amount = 1)
        {
            _cursor = Mathf.Min(_cursor + amount, _graph.width);
        }
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
