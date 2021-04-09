using System.Collections;
using System.Collections.Generic;
using UnityEngine.Experimental.Rendering;
using UnityEngine;

/// <summary>
/// Handles dispatching initialisation data and commands to the PSO simulation compute shader.
/// </summary>
public class PSOSimulatorDispatcher : MonoBehaviour
{
    /// <summary>
    /// Simulation event delegate wrapper.
    /// </summary>
    public delegate void SimulationEvent(RenderTexture output);

    private struct Particle
    {
        public Vector2 position;

        public Vector2 velocity;

        public Vector2 localBest;

        public Vector2 globalBest;
    }

    private const int OutputWidth = 512;

    private const int OutputHeight = 512;

    private const int ParticlesCount = 512;

    private ComputeBuffer _bufferParticles;

    public RenderTexture _particleSpace;

    private int _csSimulate;

    private int _csDissipate;

    private Particle[] _psoParticles = new Particle[ParticlesCount];

    private float _decay;
    
    private int _step;

    private bool _completed = false;

    private Material _background;

    /// <summary>
    /// The compute shader containing the current desired simulator.
    /// </summary>
    public ComputeShader simulator;

    /// <summary>
    /// The starting and minimum inertial coefficient.
    /// </summary>
    [Tooltip("The starting and minimum inertial coefficient."), MinMaxSlider(0, 2)]
    public Vector2 w = new Vector2(0.5f, 1.0f);

    /// <summary>
    /// The starting self-following coefficient.
    /// </summary>
    [Tooltip("The starting self-following coefficient.")]
    public float c1 = 0.02f;
    
    /// <summary>
    /// The starting social-following coefficient.
    /// </summary>
    [Tooltip("The starting social-following coefficient.")]
    public float c2 = 0.02f;
    
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
    /// Completed simulation event handler.
    /// Event will only trigger once, when the iterations counter has reached its limit.
    /// </summary>
    public event SimulationEvent simulationComplete;


#if UNITY_EDITOR
    private void Start() 
    {
        if (!simulator)
        {
            Debug.LogError("No simulator was provided. Interrupting...");
            throw new System.Exception();
        }
    }
#endif

    private unsafe void Awake()
    {
        _background = new Material(Shader.Find("Hidden/PSOBackground"));

        _decay = (1.0f - w.x) / iterations;
        _step = iterations;

        for (int i = 0; i < ParticlesCount; i += 1)
        {
            _psoParticles[i].position = new Vector2(Random.Range(0, OutputWidth), Random.Range(0, OutputHeight));
            _psoParticles[i].velocity = Random.insideUnitCircle;
            _psoParticles[i].localBest = _psoParticles[i].position;
            _psoParticles[i].globalBest = _psoParticles[i].position;
        }

        _bufferParticles = new ComputeBuffer(ParticlesCount, sizeof(Particle));
        _bufferParticles.SetData(_psoParticles);

        _particleSpace = new RenderTexture(OutputWidth, OutputHeight, sizeof(float) * 3, DefaultFormat.LDR);
        _particleSpace.enableRandomWrite = true;
        _particleSpace.Create();

        simulator.SetInt("OutputWidth", OutputWidth);
        simulator.SetInt("OutputHeight", OutputHeight);
        simulator.SetFloat("C1", c1);
        simulator.SetFloat("C2", c2);
        simulator.SetFloat("CommsDistance", commsDistance);

        _csSimulate = simulator.FindKernel("CSSimulate");
        _csDissipate = simulator.FindKernel("CSDissipate");

        simulator.SetTexture(_csSimulate, "ParticleSpace", _particleSpace, 0);
        simulator.SetTexture(_csDissipate, "ParticleSpace", _particleSpace, 0);
        simulator.SetBuffer(_csSimulate, "Particles", _bufferParticles);
    }

    private void Update()
    {
        if (_step > 0)
        {
            simulator.SetFloat("W", w.y);
            
            if (w.y > w.x)
            {
                w.y -= _decay; 
            }

            _step -= 1;

            simulator.Dispatch(_csDissipate, OutputWidth / 32, OutputHeight / 32, 1);
            simulator.Dispatch(_csSimulate, ParticlesCount / 32, ParticlesCount / 32, 1);
        }
        else if (!_completed)
        {
            simulationComplete.Invoke(_particleSpace);
            _completed = true;
        }
    }

    private void OnRenderImage(RenderTexture src, RenderTexture dest) 
    {
        Graphics.Blit(_particleSpace, dest, _background);
    }

    private void OnDestroy() 
    {
        _bufferParticles.Release();
    }
}