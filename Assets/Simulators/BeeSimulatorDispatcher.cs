using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Experimental.Rendering;

namespace Assets.Simulators
{
    /// <summary>
    /// Handles dispatching initialisation data and commands to the Bee simulation compute shader.
    /// Requires the output of the <see cref="PSOSimulatorDispatcher"/> to operate.
    /// </summary>
    public class BeeSimulatorDispatcher : MonoBehaviour
    {
        private const int DownsizeFactor = 8;

        private const int MaxOptions = 5;

        private int _step = 0;

#if DEBUG
        [SerializeField]
#endif
        private RenderTexture _output;

        private readonly Vector2Int[] _options = new Vector2Int[MaxOptions];

        private int _optionsCount = 0;

        /// <summary>
        /// A reference to the previous iteration of the simulation system.
        /// </summary>
        [Tooltip("A reference to the previous iteration of the simulation system.")]
        public PSOSimulatorDispatcher psoSimulator;

        /// <summary>
        /// How many simulation steps should be performed.
        /// </summary>
        [Tooltip("How many simulation steps should be performed.")]
        public int iterations = 1000;

        /// <summary>
        /// The distance for communications.
        /// </summary>
        [Tooltip("The distance for communications.")]
        public float commsDistance = 15.0f;


        void DetectOptions(RenderTexture pso)
        {
            List<Vector3> detectedOptions = new List<Vector3>();
            Texture2D map = new Texture2D(pso.width, pso.height);

            RenderTexture backBuffer = RenderTexture.active;
            RenderTexture.active = pso;

            map.ReadPixels(new Rect(0, 0, pso.width, pso.height), 0, 0);

            RenderTexture.active = backBuffer;

            for (int x = 0; x < map.width; x += 1)
            {
                for (int y = 0; y < map.height; y += 1)
                {
                    float cell = map.GetPixel(x, y).r;
                    if (cell > .25f)
                    {
                        detectedOptions.Add(new Vector3(x / (float)map.width, y / (float)map.height, cell));
                    }
                }
            }

            // Now sort the list based on the strongest cell values
            detectedOptions.Sort((optionA, optionB) => (int)(optionB.z - optionA.z));
            int foundEntries = Mathf.Min(detectedOptions.Count, MaxOptions);

            for (int i = 0; i < foundEntries; i += 1)
            {
                _options[i] = new Vector2Int(
                    Mathf.RoundToInt(detectedOptions[i].x),
                    Mathf.RoundToInt(detectedOptions[i].y)
                );
            }

            _optionsCount = foundEntries;

#if DEBUG
            Debug.Log($"Found a total of {_optionsCount} options, proceeding to algorithm 2...");
#endif
        }

        IEnumerator InitializationCoroutine(RenderTexture output)
        {
            _output = new RenderTexture(output.width, output.height, sizeof(float) * 3, DefaultFormat.LDR);
            RenderTexture tmp = new RenderTexture(output.width / DownsizeFactor, output.height / DownsizeFactor, sizeof(float) * 3, DefaultFormat.LDR);

            Graphics.Blit(output, tmp);

            DetectOptions(tmp);
            Destroy(psoSimulator);

            _step = iterations;
            enabled = true;

            yield return new WaitForEndOfFrame();

            _output = tmp;
        }

        void Initialize(RenderTexture output)
        {
            StartCoroutine(InitializationCoroutine(output));
        }

        void Awake()
        {
            psoSimulator.simulationComplete += Initialize;
            enabled = false;
        }

        void Update()
        {
            while (_step > 0)
            {
                _step -= 1;
            }
        }

        private void OnRenderImage(RenderTexture source, RenderTexture destination)
        {
            Graphics.Blit(_output, destination);
        }

        private void OnDestroy()
        {
            if (_output)
            {
                _output.DiscardContents();
                Destroy(_output);
            }
        }
    }
}