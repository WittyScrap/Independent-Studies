using System.Collections;
using System.Collections.Generic;
using UnityEngine.Experimental.Rendering;
using UnityEngine;

/// <summary>
/// Handles dispatching initialisation data and commands to the simulation compute shader.
/// </summary>
public class SimulatorDispatcher : MonoBehaviour
{
    private struct Particle
    {
        public Vector2 position;

        public Vector2 velocity;

        public Vector2 localBest;

        public Vector3 color;
    }

    private struct GlobalState
    {
        public Vector2 globalBest;
    }

    private const int OutputWidth = 512;

    private const int OutputHeight = 512;

    private const int ParticlesCount = 2048;

    private ComputeBuffer _bufferParticles;

    private ComputeBuffer _bufferGlobal;

    private RenderTexture _particleSpace;

    private int _csSimulate;

    private int _csDissipate;

    private Particle[] _psoParticles = new Particle[ParticlesCount];

    private GlobalState[] _global = new GlobalState[1];

    private float _decay;
    
    private int _step;

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
    public int iterations = 100000;


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

    private Vector3 RandomRGB()
    {
        Color color = Random.ColorHSV(0, 1, 0, 1, .5f, 1);
        return new Vector3(color.r, color.g, color.b);
    }

    private unsafe void Awake() 
    {
        _decay = (1.0f - w.x) / iterations;
        _step = iterations;

        for (int i = 0; i < ParticlesCount; i += 1)
        {
            _psoParticles[i].position = new Vector2(Random.Range(0, OutputWidth), Random.Range(0, OutputHeight));
            _psoParticles[i].velocity = Random.insideUnitCircle;
            _psoParticles[i].localBest = _psoParticles[i].position;
            _psoParticles[i].color = RandomRGB();
        }

        _global[0].globalBest = _psoParticles[0].localBest;

        _bufferParticles = new ComputeBuffer(ParticlesCount, sizeof(Particle));
        _bufferParticles.SetData(_psoParticles);

        _bufferGlobal = new ComputeBuffer(1, sizeof(GlobalState));
        _bufferGlobal.SetData(_global);

        _particleSpace = new RenderTexture(OutputWidth, OutputHeight, sizeof(float) * 3, DefaultFormat.LDR);
        _particleSpace.enableRandomWrite = true;
        _particleSpace.Create();

        simulator.SetInt("OutputWidth", OutputWidth);
        simulator.SetInt("OutputHeight", OutputHeight);
        simulator.SetFloat("C1", c1);
        simulator.SetFloat("C2", c2);

        _csSimulate = simulator.FindKernel("CSSimulate");
        _csDissipate = simulator.FindKernel("CSDissipate");

        simulator.SetTexture(_csSimulate, "ParticleSpace", _particleSpace, 0);
        simulator.SetTexture(_csDissipate, "ParticleSpace", _particleSpace, 0);
        simulator.SetBuffer(_csSimulate, "Particles", _bufferParticles);
        simulator.SetBuffer(_csSimulate, "Global", _bufferGlobal);
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
        }

        simulator.Dispatch(_csDissipate, OutputWidth / 32, OutputHeight / 32, 1);
        simulator.Dispatch(_csSimulate, ParticlesCount / 1024, 1, 1);
    }

    private void OnRenderImage(RenderTexture src, RenderTexture dest) 
    {
        Graphics.Blit(_particleSpace, dest);
    }

    private void OnDestroy() 
    {
        _bufferParticles.Release();
        _bufferGlobal.Release();
    }
}