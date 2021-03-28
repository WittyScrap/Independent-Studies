using System.Collections;
using System.Collections.Generic;
using UnityEngine.Experimental.Rendering;
using UnityEngine;

/// <summary>
/// Handles dispatching initialisation data and commands to the simulation compute shader.
/// </summary>
public class SimulatorDispatcher : MonoBehaviour
{
    /// <summary>
    /// Represents a single particle for the Particle Swarm Optimisation
    /// based algorithm.
    /// </summary>
    private struct Particle
    {
        public Vector2 position;

        public Vector2 velocity;
    }

    public ComputeShader simulator;

    private ComputeBuffer _bufferParticles;

    private const int OutputWidth = 1024;

    private const int OutputHeight = 1024;

    private const int ParticlesCount = 65536;

    private RenderTexture _particleSpace;

    private int _csSimulate;

    private int _csDissipate;

    private Particle[] _psoParticles = new Particle[ParticlesCount];


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
        for (int i = 0; i < ParticlesCount; i += 1)
        {
            _psoParticles[i].position = new Vector2(Random.Range(0, OutputWidth), Random.Range(0, OutputHeight));
            _psoParticles[i].velocity = Random.insideUnitCircle;
        }

        _bufferParticles = new ComputeBuffer(ParticlesCount, sizeof(Particle));
        _bufferParticles.SetData(_psoParticles);

        _particleSpace = new RenderTexture(OutputWidth, OutputHeight, 4, DefaultFormat.LDR);
        _particleSpace.enableRandomWrite = true;
        _particleSpace.Create();

        simulator.SetInt("OutputWidth", OutputWidth);
        simulator.SetInt("OutputHeight", OutputHeight);

        _csSimulate = simulator.FindKernel("CSSimulate");
        _csDissipate = simulator.FindKernel("CSDissipate");

        simulator.SetTexture(_csSimulate, "ParticleSpace", _particleSpace, 0);
        simulator.SetTexture(_csDissipate, "ParticleSpace", _particleSpace, 0);
        simulator.SetBuffer(_csSimulate, "Particles", _bufferParticles);
    }

    private void Update()
    {
        simulator.Dispatch(_csSimulate, ParticlesCount / 1024, 1, 1);
        simulator.Dispatch(_csDissipate, OutputWidth / 32, OutputHeight / 32, 1);
    }

    private void OnRenderImage(RenderTexture src, RenderTexture dest) 
    {
        Graphics.Blit(_particleSpace, dest);
    }

    private void OnDestroy() 
    {
        _bufferParticles.Release();
    }
}