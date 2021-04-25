using System;
using System.Linq;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEditor;
using Int32 = System.Int32;
using Single = System.Single;

namespace Simulators
{
    /// <summary>
    /// Handles dispatching initialisation data and commands to the Bee simulation compute shader.
    /// Requires the output of the <see cref="PSOSimulatorDispatcher"/> to operate.
    /// </summary>
    public sealed class BeeSimulatorDispatcher : MonoBehaviour
    {
        struct Particle
        {
            public Vector2      position;
            
            public Vector2      velocity;

            public Int32        preference;

            public Single       strength;

            public Single       stubborness;
        };

        private const int MaxOptions = 5;

        private const int ParticlesCount = 1024;

        private static readonly Color[] OptionsColors = new Color[MaxOptions]
        {
            Color.red, Color.green, Color.blue, Color.yellow, Color.cyan
        };

        private static readonly Color MarkerColor = Color.white;

        private readonly Vector4[] _options = new Vector4[MaxOptions];

        private readonly Particle[] _particles = new Particle[ParticlesCount];

        private static Texture2D _markerTexture;

        private static GUIStyle _markerStyle;

        private UI.GraphWindow _graph;

        private int _optionsCount = 0;

        private Material _background;

        private int _csSimulate;

        private int _csDissipate;

        private int _step = 0;

        private float _backgroundFade = 0;

        private Vector2 _finalLocation = -Vector2.one * 1000;

        private bool _hasFinalLocation = false;

#if DEBUG
        [SerializeField]
#endif
        private RenderTexture _particleSpace;

        private ComputeBuffer _bufferParticles;

        private ComputeBuffer _bufferOptions;

        private ComputeBuffer _bufferColors;

        /// <summary>
        /// The compute shader containing the current desired simulator.
        /// </summary>
        public ComputeShader simulator;

        /// <summary>
        /// How many simulation steps should be performed.
        /// </summary>
        [Tooltip("How many simulation steps should be performed.")]
        public int iterations = 1500;

        /// <summary>
        /// The distance for communications.
        /// </summary>
        [Tooltip("The distance for communications.")]
        public float commsDistance = 1.0f;
        
        /// <summary>
        /// A strength multiplier for communications.
        /// </summary>
        [Tooltip("The distance for communications."), Range(0, 1)]
        public float commsMultiplier = 1.0f;
        
        /// <summary>
        /// The starting and minimum inertial coefficient.
        /// </summary>
        [Tooltip("The minimum and maximum stubborness modifiers."), MinMaxSlider(-0.1f, 0.1f)]
        public Vector2 stubborness = new Vector2(-0.01f, 0.01f);

        /// <summary>
        /// The frequency by which a cross-inhibitory signal should be emitted.
        /// </summary>
        [Tooltip("The frequency by which a cross-inhibitory signal should be emitted."), Range(0, 1)]
        public float crossInhibitoryFrequence = 0.5f;

        /// <summary>
        /// Whether or not the simulation should be paused when this
        /// stage is initialized.
        /// </summary>
        [Tooltip("Whether or not the simulation should be paused when this stage is initialized.")]
        public bool pauseOnCarryover = false;


        private unsafe bool DetectOptions(Texture2D pso)
        {
            List<Vector3> detectedOptions = new List<Vector3>(pso.width * pso.height);
            float PresenceThreshold = 0.1f;

            for (int x = 0; x < pso.width; x += 1)
            {
                for (int y = 0; y < pso.height; y += 1)
                {
                    float cell = pso.GetPixel(x, y).r;

                    if (cell > PresenceThreshold)
                    {
                        detectedOptions.Add(new Vector3(x / (float)pso.width, y / (float)pso.height, cell));
                    }
                }
            }

            // Now sort the list based on the strongest cell values
            detectedOptions = detectedOptions.OrderByDescending(option => option.z).ToList();
            int foundEntries = Mathf.Min(detectedOptions.Count, MaxOptions);

            for (int i = 0; i < foundEntries; i += 1)
            {
                _options[i] = new Vector4(
                    detectedOptions[i].x,
                    detectedOptions[i].y
                );
            }

            _optionsCount = foundEntries;

            if (_optionsCount <= 1)
            {
#if DEBUG
                Debug.Log("Only one (or no) option(s) detected, terminating...");

                if (_optionsCount == 1)
                {
                    _finalLocation = new Vector2(-1, 1) - (Vector2)_options[0];
                    Debug.Log($"Final position: {_finalLocation}; Final value: {detectedOptions[0].z}");
				}
                else
				{
                    Debug.Log("No options detected.");
				}
#endif
                return false;
            }

#if DEBUG
            Debug.Log($"Found a total of {_optionsCount} options out of a maximum of {MaxOptions}, refining output...");
#endif

            _background.SetVectorArray("_Options", _options);
            _background.SetColorArray("_Colors", OptionsColors);
            _background.SetInt("_TotalOptions", _optionsCount);
            _background.SetInt("_OutputSize", _particleSpace.width);

            const float SpawnRadius = 50;

            // Initialize particles
            for (int i = 0; i < ParticlesCount; i += 1)
            {
                // Increase entropy by increasing the number of possible outcomes
                int choice = UnityEngine.Random.Range(0, _optionsCount * 50) % _optionsCount;

                _particles[i].position = _particleSpace.GetSize() / 2 + UnityEngine.Random.insideUnitCircle * SpawnRadius;
                _particles[i].velocity = VectorExtensions.RandomUnitCircumference();
                _particles[i].preference = choice;
                _particles[i].strength = UnityEngine.Random.value;
                _particles[i].stubborness = UnityEngine.Random.Range(stubborness.x, stubborness.y);
            }


#if DEBUG
            Debug.Log($"{ParticlesCount} particles initialized; preparing simulator shader...");
#endif

            _bufferParticles = new ComputeBuffer(ParticlesCount, sizeof(Particle));
            _bufferParticles.SetData(_particles);

            _bufferOptions = new ComputeBuffer(MaxOptions, sizeof(Vector4));
            _bufferOptions.SetData(_options);

            _bufferColors = new ComputeBuffer(MaxOptions, sizeof(Color));
            _bufferColors.SetData(OptionsColors);

            simulator.SetFloat("CommsDistance", commsDistance);
            simulator.SetFloat("SpawnRadius", SpawnRadius);
            simulator.SetFloat("CommsStrength", commsMultiplier);
            simulator.SetFloat("CrossInhibitoryFrequence", crossInhibitoryFrequence);
            simulator.SetInt("ParticlesCount", ParticlesCount);
            simulator.SetInt("OutputWidth", _particleSpace.width);
            simulator.SetInt("OutputHeight", _particleSpace.height);

            _csSimulate = simulator.FindKernel("CSSimulate");
            _csDissipate = simulator.FindKernel("CSDissipate");

            simulator.SetTexture(_csSimulate, "ParticleSpace", _particleSpace, 0);
            simulator.SetTexture(_csDissipate, "ParticleSpace", _particleSpace, 0);
            simulator.SetBuffer(_csSimulate, "Particles", _bufferParticles);
            simulator.SetBuffer(_csSimulate, "Options", _bufferOptions);
            simulator.SetBuffer(_csSimulate, "OptionsColors", _bufferColors);

#if DEBUG
            Debug.Log("Simulator shader ready.");
#endif 

            return true;
        }

        private void Blit(Texture2D destination, RenderTexture source)
		{
            RenderTexture temporary = RenderTexture.GetTemporary(destination.width, destination.height);
            RenderTexture.active = temporary;
            Graphics.Blit(source, temporary);

            destination.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            RenderTexture.ReleaseTemporary(temporary);
        }

        /// <summary>
        /// Initializes the simulator from the output of the previous simulation
        /// step (<see cref="PSOSimulatorDispatcher"/>).
        /// </summary>
        public void Initialize(RenderTexture output)
        {
            UnityEngine.Random.InitState(Guid.NewGuid().GetHashCode());

            _particleSpace = new RenderTexture(output.width, output.height, sizeof(float) * 3, DefaultFormat.LDR)
            {
                enableRandomWrite = true
            };
            
            const int DownsizeFactor = 4;

            Texture2D pso = new Texture2D
            (
                width: output.width / DownsizeFactor, 
                height: output.height / DownsizeFactor
            );

            Blit(pso, output);

            if (DetectOptions(pso))
            {
#if DEBUG
                if (pauseOnCarryover)
				{
                    Debug.Break();
				}
#endif
                _step = iterations;
                _backgroundFade = 0;
                
                _graph = EditorWindow.GetWindow<UI.GraphWindow>();
                _graph.Initialize(new Rect(0, 0, 700, 200), iterations, "Time", "Predominance");
            }

            Destroy(pso);
            enabled = true;
        }

        private void FindFinalLocation()
		{
            if (_bufferParticles != null)
            {
                _bufferParticles.GetData(_particles);

                int[] votes = new Int32[MaxOptions];

                for (int i = 0; i < ParticlesCount; i += 1)
                {
                    votes[_particles[i].preference] += 1;
                }

                int highestIndex = 0;

                for (int i = 0; i < MaxOptions; i += 1)
				{
                    if (votes[i] > votes[highestIndex])
					{
                        highestIndex = i;
					}
				}

                _finalLocation = _options[highestIndex];

#if DEBUG
                Debug.Log($"Final location: {_finalLocation}");
#endif
            }
		}

        private void AdvanceGraph()
        {
            if (_graph != null && _bufferParticles != null)
            {
                _bufferParticles.GetData(_particles);
                var votes = new Single[_optionsCount];

                for (int i = 0; i < ParticlesCount; i += 1)
                {
                    votes[_particles[i].preference] += 1;
                }

                for (int i = 0; i < _optionsCount; i += 1)
                {
                    _graph.Record(votes[i] / ParticlesCount, OptionsColors[i]);
                }

                _graph.Advance();
            }
        }

        public void Awake()
        {
            _background = new Material(Shader.Find("Hidden/BeesBackground"));
            enabled = false;

            if (!_markerTexture)
			{
                _markerTexture = new Texture2D(1, 1);
                _markerTexture.SetPixel(0, 0, MarkerColor);
                _markerStyle = new GUIStyle();
                _markerStyle.normal.background = _markerTexture;
			}
        }

        public void Update()
        {
            _background.SetFloat("_Fade", _backgroundFade);
            _backgroundFade += Time.deltaTime;

            simulator.SetInt("Step", _step);
            simulator.Dispatch(_csDissipate, _particleSpace.width / 32, _particleSpace.height / 32, 1);
            simulator.Dispatch(_csSimulate, ParticlesCount / 512, 1, 1);

            if (_step > 0)
            {
                AdvanceGraph();
            }

            if (_step <= 0 && !_hasFinalLocation)
			{
                FindFinalLocation();
                _hasFinalLocation = true;
			}
            
            _step -= 1;
        }

		public void OnGUI()
		{
            const int CrosshairSize = 50;
            const int CrosshairThickness = 1;

            float pointX = (_finalLocation.x * Screen.width);
            float pointY = ((1 - _finalLocation.y) * Screen.height);

            float startX = pointX - (CrosshairSize / 2);
            float startY = pointY - (CrosshairSize / 2);

            GUI.Box(new Rect(pointX, startY, CrosshairThickness, CrosshairSize + CrosshairThickness), string.Empty, _markerStyle);
            GUI.Box(new Rect(startX, pointY, CrosshairSize + CrosshairThickness, CrosshairThickness), string.Empty, _markerStyle);
            
            GUILayout.BeginArea(new Rect(0, 0, Screen.width, Screen.height));
            GUILayout.BeginHorizontal();
            GUILayout.FlexibleSpace();
            GUILayout.BeginVertical();
            GUILayout.FlexibleSpace();

            if (!_hasFinalLocation)
            {
                GUILayout.Label("PHASE 2 : CONSENSUS SIMULATION");
            }
            else
            {
                GUILayout.Label($"OPTIMUM = {_finalLocation}");
            }
            
            GUILayout.EndVertical();
            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();
            GUILayout.EndArea();
		}

		public void OnRenderImage(RenderTexture source, RenderTexture destination)
        {
            Graphics.Blit(_particleSpace, destination, _background);
        }

        public void OnDestroy()
        {
            if (_particleSpace)
            {
                _particleSpace.DiscardContents();
                Destroy(_particleSpace);
            }

            if (_bufferParticles != null)
            {
                _bufferParticles.Dispose();
            }

            if (_bufferOptions != null)
			{
                _bufferOptions.Dispose();
			}

            if (_bufferColors != null)
			{
                _bufferColors.Dispose();
			}
        }
    }
}