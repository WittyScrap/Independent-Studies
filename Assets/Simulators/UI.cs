using UnityEngine.SceneManagement;
using UnityEngine;
using UnityEditor;
using System.Linq;

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
        public string XAxis { get; private set; }

        /// <summary>
        /// The label to assign to the <c>Y</c> axis.
        /// </summary>
        public string YAxis { get; private set; }

        private const float FrameSize = 10;

        private const int LabelsMargin = 30;

        private Rect _window;

        private Texture2D _graph;

        private int _cursor = 0;

        
        public void Initialize(in Rect window, in string xAxis, in string yAxis)
        {
            XAxis = xAxis;
            YAxis = yAxis;
            
            _window = window;

            minSize = _window.size + new Vector2(0, LabelsMargin);
            maxSize = minSize;

            int width = Mathf.RoundToInt(_window.width);
            int height = Mathf.RoundToInt(_window.height);

            _graph = new Texture2D(width, height);
        }

        public void OnGUI()
        {
            if (_graph)
            {
                GUI.Box(
                    position:       new Rect(Vector2.zero, position.size),
                    image:          _graph
                );
            }
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
            int y = Mathf.RoundToInt(atY * _window.height);
            _graph.SetPixel(_cursor, y, color);
        }

        /// <summary>
        /// Advances the X axis cursor by the provided amount (default: 1).
        /// </summary>
        public void Advance(in int amount = 1)
        {
            _cursor = Mathf.Min(_cursor + amount, _graph.width);
            _graph.Apply();
            Repaint();
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
