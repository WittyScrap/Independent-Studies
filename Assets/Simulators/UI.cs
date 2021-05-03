using UnityEngine.SceneManagement;
using UnityEngine;
using UnityEditor;
using System.Linq;
using System.IO;
using static System.Uri;

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

        private Rect _window;

        private Texture2D _graph;

        private int _cursor = 0;

        private int _range;

        private int _filler = 0;

        private int _step = 1;


        [MenuItem("Simulation/Progress Graph")]
        private static void Create()
        {
            EditorWindow.GetWindow<GraphWindow>().Show();
        }
        
        public void Initialize(in Rect window, in int range, in string xAxis, in string yAxis,
            in int expand = 1)
        {
            _cursor = 0;

            XAxis = xAxis;
            YAxis = yAxis;

            _window = window;
            _range = range;

            if (_graph != null)
            {
                Destroy(_graph);
            }

            _graph = new Texture2D(range * expand, Mathf.RoundToInt(_window.height));
            titleContent = new GUIContent("Progress Graph");

            _step = expand;
        }

        /// <summary>
        /// Saves the contents of this graph as a picture.
        /// </summary>
        public void Save()
        {
            string path = EditorUtility.SaveFilePanel
            (
                title: "Save graph as PNG",
                directory: "",
                defaultName: "graph.png",
                extension: "png"
            );

            if (IsWellFormedUriString(path, System.UriKind.Absolute))
            {
                byte[] texture = _graph.EncodeToPNG();
                File.WriteAllBytes(path, texture);
            }
        }

        public void OnGUI()
        {
            if (_graph)
            {
                const float BorderRadius = 10;

                GUI.DrawTexture(
                    position:       new Rect(Vector2.zero, position.size),
                    image:          _graph,
                    scaleMode:      ScaleMode.ScaleToFit,
                    alphaBlend:     false,
                    imageAspect:    0,
                    color:          Color.white,
                    borderWidth:    0,
                    borderRadius:   BorderRadius
                );

                Color previous = GUI.contentColor;
                GUI.contentColor = Color.black;

                // X-Axis label
                GUILayout.BeginHorizontal();
                GUILayout.FlexibleSpace();
                GUILayout.BeginVertical();
                GUILayout.FlexibleSpace();
                GUILayout.Label(XAxis);
                GUILayout.EndVertical();
                GUILayout.FlexibleSpace();
                GUILayout.EndHorizontal();
                
                GUI.contentColor = previous;
            }
            else
            {
                GUILayout.BeginHorizontal();
                GUILayout.FlexibleSpace();
                GUILayout.BeginVertical();
                GUILayout.FlexibleSpace();
                GUILayout.Label("No simulation data");
                GUILayout.FlexibleSpace();
                GUILayout.EndVertical();
                GUILayout.FlexibleSpace();
                GUILayout.EndHorizontal();
            }
        }

        public void OnDestroy()
        {
            DestroyImmediate(_graph);
        }

        /// <summary>
        /// Plots a new entry at the current cursor position, without advancing it.
        /// </summary>
        public void Record(in float amount, in Color color)
        {
            int pixels = Mathf.RoundToInt(amount * _window.height);
            int overflow = (_filler + pixels) - _graph.height;

            if (overflow > 0)
            {
                pixels -= overflow;
            }

            Color[] colors = Enumerable.Repeat(color, pixels).ToArray();
            Color brightness = new Color(0.25f, 0.25f, 0.25f, 1.0f);

            for (int i = 0; i < colors.Length; i += 1)
            {
                colors[i] += Color.grey + (brightness * (1 - (float)i / colors.Length));
            }

            _graph.SetPixels(_cursor, _filler, 1, pixels, colors, 0);
            _filler = Mathf.Min(_filler + pixels, _graph.height);
        }

        /// <summary>
        /// Advances the X axis cursor by the provided amount (default: 1).
        /// </summary>
        public void Advance(in int amount = 1)
        {
            _filler = 0;
            _cursor = Mathf.Min(_cursor + amount * _step, _graph.width);
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
