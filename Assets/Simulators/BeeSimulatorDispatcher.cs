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
        struct Particle
        {
            public Vector2 position;
            
            public Vector2 velocity;

            public Vector3 preference;
        };

        private const int DownsizeFactor = 8;

        private const int MaxOptions = 5;

        private const int ParticlesCount = 512;

        private int _step = 0;

#if DEBUG
        [SerializeField]
#endif
        private RenderTexture _particleSpace;

        private ComputeBuffer _bufferParticles;

        private readonly Vector4[] _options = new Vector4[MaxOptions];

        private readonly Particle[] _particles = new Particle[ParticlesCount];

        private int _optionsCount = 0;

        private Material _background;

        private int _csSimulate;

        private int _csDissipate;

        /// <summary>
        /// The compute shader containing the current desired simulator.
        /// </summary>
        public ComputeShader simulator;

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


        private unsafe void DetectOptions(Texture2D pso)
        {
            List<Vector3> detectedOptions = new List<Vector3>();

            for (int x = 0; x < pso.width; x += 1)
            {
                for (int y = 0; y < pso.height; y += 1)
                {
                    float cell = pso.GetPixel(x, y).r;

                    if (cell > .25f)
                    {
                        detectedOptions.Add(new Vector3(x / (float)pso.width, y / (float)pso.height, cell));
                    }
                }
            }

            // Now sort the list based on the strongest cell values
            detectedOptions.Sort((optionA, optionB) => (int)(optionB.z - optionA.z));
            int foundEntries = Mathf.Min(detectedOptions.Count, MaxOptions);

            for (int i = 0; i < foundEntries; i += 1)
            {
                _options[i] = new Vector4(
                    detectedOptions[i].x,
                    detectedOptions[i].y
                );
            }

            _optionsCount = foundEntries;

#if DEBUG
            Debug.Log($"Found a total of {_optionsCount} options out of a maximum of {MaxOptions}, refining output...");
#endif

            _background.SetVectorArray("_Options", _options);
            _background.SetInt("_TotalOptions", _optionsCount);

            // Initialize particles
            for (int i = 0; i < ParticlesCount; i += 1)
			{
                _particles[i].position = _options[Random.Range(0, _optionsCount)] * new Vector2(_particleSpace.width, _particleSpace.height);
                _particles[i].velocity = Random.insideUnitCircle;
                _particles[i].preference = new Vector3(_particles[i].position.x, _particles[i].position.y, Random.value);
			}

#if DEBUG
            Debug.Log($"{ParticlesCount} particles initialized; preparing simulator shader...");
#endif

            _bufferParticles = new ComputeBuffer(ParticlesCount, sizeof(Particle));
            _bufferParticles.SetData(_particles);

            simulator.SetFloat("CommsDistance", commsDistance);

            _csSimulate = simulator.FindKernel("CSSimulate");
            _csDissipate = simulator.FindKernel("CSDissipate");

            simulator.SetTexture(_csSimulate, "ParticleSpace", _particleSpace, 0);
            simulator.SetTexture(_csDissipate, "ParticleSpace", _particleSpace, 0);
            simulator.SetBuffer(_csSimulate, "Particles", _bufferParticles);

#if DEBUG
            Debug.Log("Simulator shader ready.");
#endif
        }

        private void Blit(Texture2D destination, RenderTexture source)
		{
            RenderTexture temporary = RenderTexture.GetTemporary(destination.width, destination.height);
            RenderTexture.active = temporary;
            Graphics.Blit(source, temporary);

            destination.ReadPixels(new Rect(0, 0, destination.width, destination.height), 0, 0);
            RenderTexture.ReleaseTemporary(temporary);
        }

        private void Initialize(RenderTexture output)
        {
            _particleSpace = new RenderTexture(output.width, output.height, sizeof(float) * 3, DefaultFormat.LDR)
            {
                enableRandomWrite = true
            };

            Texture2D pso = new Texture2D
            (
                width: output.width / DownsizeFactor, 
                height: output.height / DownsizeFactor
            );

            Blit(pso, output);
            DetectOptions(pso);

            Destroy(psoSimulator);
            Destroy(pso);

            _step = iterations;
            enabled = true;
        }

        private void Awake()
        {
            _background = new Material(Shader.Find("Hidden/BeesBackground"));
            psoSimulator.SimulationComplete += Initialize;
            enabled = false;
        }

        private void Update()
        {
            while (_step > 0)
            {
                simulator.Dispatch(_csDissipate, ParticlesCount / 32, ParticlesCount / 32, 1);
                simulator.Dispatch(_csSimulate, ParticlesCount / 32, ParticlesCount / 32, 1);

                _step -= 1;
            }
        }

        private void OnRenderImage(RenderTexture source, RenderTexture destination)
        {
            Graphics.Blit(_particleSpace, destination, _background);
        }

        private void OnDestroy()
        {
            if (_particleSpace)
            {
                _particleSpace.DiscardContents();
                Destroy(_particleSpace);
            }

            _bufferParticles.Dispose();
        }
    }
}