﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
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
        };

        private const int MaxOptions = 5;

        private const int ParticlesCount = 1024;

        private static readonly Color[] OptionsColors = new Color[MaxOptions]
        {
            Color.red, Color.green, Color.blue, Color.yellow, Color.cyan
        };

        private readonly Vector4[] _options = new Vector4[MaxOptions];

        private readonly Particle[] _particles = new Particle[ParticlesCount];

        private int _optionsCount = 0;

        private Material _background;

        private int _csSimulate;

        private int _csDissipate;

        private int _step = 0;

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
        public int iterations = 1000;

        /// <summary>
        /// The distance for communications.
        /// </summary>
        [Tooltip("The distance for communications.")]
        public float commsDistance = 15.0f;

        /// <summary>
        /// Whether or not the simulation should be paused when this
        /// stage is initialized.
        /// </summary>
        [Tooltip("Whether or not the simulation should be paused when this stage is initialized.")]
        public bool pauseOnCarryover = false;


        private unsafe bool DetectOptions(Texture2D pso)
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

            if (_optionsCount <= 1)
            {
#if DEBUG
                Debug.Log("Only one (or no) option(s) detected, terminating...");

                if (_optionsCount == 1)
				{
                    Debug.Log($"Final position: {new Vector2(_options[0].x, _options[0].y) * pso.GetSize()}; Final value: {detectedOptions[0].z}");
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

            const float SpawnRadius = 50;

            // Initialize particles
            for (int i = 0; i < ParticlesCount; i += 1)
            {
                int choice = Random.Range(0, _optionsCount);

                _particles[i].position = _particleSpace.GetSize() / 2 + Random.insideUnitCircle * SpawnRadius;
                _particles[i].velocity = VectorExtensions.RandomUnitCircumference();
                _particles[i].preference = choice;
                _particles[i].strength = Random.value;
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
            _particleSpace = new RenderTexture(output.width, output.height, sizeof(float) * 3, DefaultFormat.LDR)
            {
                enableRandomWrite = true
            }; 
            
            const int DownsizeFactor = 8;

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

                enabled = true;
                _step = iterations;
            }

            Destroy(pso);
        }

        private void Awake()
        {
            _background = new Material(Shader.Find("Hidden/BeesBackground"));
            enabled = false;
        }

        private void Update()
        {
            if (_step > 0)
            {
                simulator.Dispatch(_csDissipate, _particleSpace.width / 32, _particleSpace.height / 32, 1);
                simulator.Dispatch(_csSimulate, ParticlesCount / 512, 1, 1);

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